// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayMainWidgetBase.h"
#include "GameObjects/GameObjectCollection.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/Enemies/enemies.h"
#include "Common/Painter.h"
#include "DefconUtils.h"
#include "DefconLogging.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



//const float StargateExpanderLifetime = 2.5f; // Same as FADE_DURATION_NORMAL in playviewbase.h

// todo: move to common/utils_core.h
// Function wrapper around check() macro so that other macros can call check() w/o expansion problems
static void Check(bool Condition) { check(Condition); }


void UDefconPlayMainWidgetBase::Init
(
	Defcon::CGameObjectCollection* InHumans, 
	Defcon::CGameObjectCollection* InObjects, 
	Defcon::CGameObjectCollection* InEnemies, 
	Defcon::CGameObjectCollection* InDebris,
	Defcon::CGameObjectCollection* InBlasts,
	const FVector2D& InArenaSize
)
{
	Humans    = InHumans;
	Objects   = InObjects;
	Enemies   = InEnemies;
	Debris    = InDebris;
	Blasts    = InBlasts;
	ArenaSize = InArenaSize;

	ClearMessages();
}


void UDefconPlayMainWidgetBase::NativeOnInitialized()
{
	LOG_UWIDGET_FUNCTION
	Super::NativeOnInitialized();

	// For the duration of the playviewbase, all playobject widgets will be installed/uninstalled on our canvas.

	Daylon::SetRootCanvas(RootCanvas);
	Daylon::SetWidgetTree(WidgetTree);

	// Load up the GameObjectResources global.

	GGameObjectResources.SmartbombBrushPtr     = &SmartbombBrush;
	GGameObjectResources.DebrisBrushRoundPtr   = &DebrisBrushRound;
	GGameObjectResources.DebrisBrushSquarePtr  = &DebrisBrushSquare;

	// Our textures were 3x the arcade, but need to be 4.25x larger. So for now scale the widget sizes that much.
	// Note: player ship is already okay. Once we have our 4K textures the factor should be 0.5f.
	const auto UpscaleFactor = 4.25f / 3.0f;

#define ADD_ATLAS(_ObjType, _Atlas) \
	Check(_Atlas != nullptr); /* Did you forget to add Atlas asset to UUserWidget's texture list? */	\
	_Atlas->Atlas.InitCache();	\
	GGameObjectResources.Add(Defcon::EObjType::_ObjType, { _Atlas, _Atlas->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f });

	ADD_ATLAS(STARGATE,         StargateAtlas           );
	ADD_ATLAS(DESTROYED_PLAYER, DestroyedPlayerAtlas    );
	ADD_ATLAS(BULLET_ROUND,     BulletAtlas             );
	ADD_ATLAS(BULLET_THIN,      ThinBulletAtlas         );
	ADD_ATLAS(FIREBALL,         FireballAtlas           );
	ADD_ATLAS(EXPLOSION,        ExplosionAtlas          );
	ADD_ATLAS(BEACON,           BeaconAtlas             );
	ADD_ATLAS(LANDER,           LanderAtlas             );
	ADD_ATLAS(BAITER,           BaiterAtlas             );
	ADD_ATLAS(BOMBER,           BomberAtlas             );
	ADD_ATLAS(BOMBER_LEFT,      BomberLeftAtlas         );
	ADD_ATLAS(FIREBOMBER_TRUE,  FirebomberAtlas         );
	ADD_ATLAS(FIREBOMBER_WEAK,  WeakFirebomberAtlas     );
	ADD_ATLAS(MINE,             MineAtlas               );
	ADD_ATLAS(POD,              PodAtlas                );
	ADD_ATLAS(SWARMER,          SwarmerAtlas            );
	ADD_ATLAS(DYNAMO,           DynamoAtlas             );
	ADD_ATLAS(SPACEHUM,         SpacehumAtlas           );
	ADD_ATLAS(PHRED,            PhredAtlas              );
	ADD_ATLAS(BIGRED,           BigRedAtlas             );
	ADD_ATLAS(MUNCHIE,          MunchieAtlas            );
	ADD_ATLAS(GUPPY,            GuppyAtlas              );
	ADD_ATLAS(HUNTER,           HunterAtlas             );
	ADD_ATLAS(GHOSTPART,        GhostPartAtlas          );
	ADD_ATLAS(REFORMERPART,     ReformerPartAtlas       );

	check(PlayerShipExhaustAtlas);	
	PlayerShipExhaustAtlas->Atlas.InitCache();

#undef ADD_ATLAS

	Messages->Clear();
}


void UDefconPlayMainWidgetBase::OnFinishActivating()
{
	LOG_UWIDGET_FUNCTION

	check(PlayerShipPtr != nullptr);
	check(Humans != nullptr);
	
	m_bHumansInMission = GDefconGameInstance->GetMission()->HumansInvolved();

	Stars.Empty();
	Stars.Reserve(STARS_COUNT);

	for(int32 Idx = 0; Idx < STARS_COUNT; Idx++)
	{
		FStar Star;

		const float X = Daylon::FRandRange(0.0f, ArenaSize.X);

		const auto Elev = (TerrainPtr == nullptr ? 0.0f : TerrainPtr->GetElev(X / ArenaSize.X) + 20);

		Star.P.Set(X, Daylon::FRandRange(Elev, ArenaSize.Y));

		Stars.Add(Star);
	}

	PlayerShipExhaust =	Daylon::SpawnSpritePlayObject2D(PlayerShipExhaustAtlas->Atlas, FVector2D(5, 5), 0.5f);
	PlayerShipExhaust.Pin()->Hide();

	PlayerShipPtr->InstallSprite();

	if(m_bHumansInMission)
	{
		Humans->ForEach([](Defcon::IGameObject* Human) { Human->InstallSprite(); });
	}
}


void UDefconPlayMainWidgetBase::OnDeactivate()
{
	LOG_UWIDGET_FUNCTION

	check(PlayerShipPtr != nullptr);
	check(Humans != nullptr);

	Daylon::Uninstall(PlayerShipExhaust.Pin());
    PlayerShipPtr->UninstallSprite();

	if(m_bHumansInMission)
	{
		Humans->ForEach([](Defcon::IGameObject* Human) { Human->UninstallSprite(); });
	}

	bDoneActivating = false;
	bSafeToStart    = false;
}


void UDefconPlayMainWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);

	// Wait until we've been initialized by the play arena.
	if(CoordMapperPtr == nullptr || PlayerShipPtr == nullptr)
	{
		return;
	}

	if(!bDoneActivating && bSafeToStart && Daylon::GetRootCanvas()->GetCanvasWidget())
	{
		OnFinishActivating();
		bDoneActivating = true;
		return;
	}


	UpdatePlayerShip(DeltaTime);

	for(auto& Star : Stars)
	{
		Star.Update(DeltaTime);
	}
}


void UDefconPlayMainWidgetBase::AddMessage(const FString& Str, float Duration)
{
	Messages->AddMessage(Str, Duration);
}


void UDefconPlayMainWidgetBase::ClearMessages()
{
	Messages->Clear();
}


void UDefconPlayMainWidgetBase::UpdatePlayerShip(float DeltaTime)
{
	// The player ship is not part of the game objects array, so process its sprite here.

	check(PlayerShipPtr != nullptr && PlayerShipPtr->Sprite);

	if(PlayerShipPtr->Sprite == nullptr)
	{
		return;
	}
	
	CFPoint pt;
	CoordMapperPtr->To(PlayerShipPtr->Position, pt);
	PlayerShipPtr->Sprite->SetPosition(FVector2D(pt.x, pt.y));
	PlayerShipPtr->Sprite->Update(DeltaTime);


	auto PlayerShipExhaustPtr = PlayerShipExhaust.Pin();

	if(PlayerShipExhaustPtr)
	{
		PlayerShipExhaustPtr->Update(DeltaTime);

		float ExhaustLength = 0.0f;

		if(PlayerShipPtr && PlayerShipPtr->IsAlive())
		{
			ExhaustLength =  ABS(3.0f * PlayerShipPtr->Inertia.x);
		}

		if(ExhaustLength == 0.0f)
		{
			PlayerShipExhaustPtr->Hide();
		}
		else
		{
			const bool IsFacingRight = (PlayerShipPtr->Orientation.Fwd.x > 0);
			auto S = PlayerShipExhaustPtr->GetActualSize();
			S.X = ExhaustLength;
			PlayerShipExhaustPtr->FlipHorizontal = !IsFacingRight;
			PlayerShipExhaustPtr->SetSize(S);
			PlayerShipExhaustPtr->UpdateWidgetSize();

			const auto PlayerShipHalfWidth = PlayerShipPtr->Sprite->GetActualSize().X / 2;
			const auto HalfExhaustLength = S.X / 2;
			FVector2D P = PlayerShipPtr->Sprite->GetPosition();

			if(IsFacingRight)
			{
				P.X -= (PlayerShipHalfWidth + HalfExhaustLength);
			}
			else
			{
				P.X += (PlayerShipHalfWidth + HalfExhaustLength);
			}

			PlayerShipExhaustPtr->SetPosition(P);
			PlayerShipExhaustPtr->Show();
		}
	}
}


void UDefconPlayMainWidgetBase::DrawObjectBbox(Defcon::IGameObject* Object, FPainter& Painter) const
{
	CFPoint pt;
	CoordMapperPtr->To(Object->Position, pt);

	CFRect r(pt);
	r.Inflate(Object->BboxRadius);

	if(r.UR.x > 0 && r.LL.x < Painter.AllottedGeometry->GetLocalSize().X)
	{
		Painter.ColorRect(r.LL.x, r.LL.y, r.UR.x, r.UR.y, C_RED);
	}
}


void UDefconPlayMainWidgetBase::DrawObjects(const Defcon::CGameObjectCollection* Collection, FPainter& Painter) const
{
	if(Collection == nullptr)
	{
		return;
	}

	Collection->ForEach([&](Defcon::IGameObject* Object)
		{
			if(!Object->Sprite)
			{
				Object->Draw(Painter, *CoordMapperPtr);
			}

			if(bShowBoundingBoxes)
			{
				DrawObjectBbox(Object, Painter);
			}
		}
	);
}


int32 UDefconPlayMainWidgetBase::NativePaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	LOG_UWIDGET_FUNCTION
	LayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);

	if(CoordMapperPtr == nullptr)
	{
		return LayerId;
	}

	if(bShowOrigin)
	{
		// debugging: draw vertical line showing zero point
		CFPoint ZeroPt(0.0f, 0.0f);
		CFPoint pt;
		CoordMapperPtr->To(ZeroPt, pt);
		TArray<FVector2f> LinePts;
		LinePts.Add(FVector2f(pt.x, 0.0f));
		LinePts.Add(FVector2f(pt.x, 900.0f));

		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), LinePts, ESlateDrawEffect::None, C_WHITE, true, 2.0f);
	}

	FPainter Painter;

	auto PaintGeometry = AllottedGeometry.ToPaintGeometry();

	Painter.PaintGeometry    = &PaintGeometry;
	Painter.AllottedGeometry = &AllottedGeometry;
	Painter.Args             = &Args;
	Painter.LayerId          = LayerId;
	Painter.MyCullingRect    = &MyCullingRect;
	Painter.OutDrawElements  = &OutDrawElements;
	Painter.RenderOpacity    = InWidgetStyle.GetColorAndOpacityTint().A;


	// Draw the stars underneath everything.

	CFPoint pt;

	for(const auto& Star : Stars)
	{
		CoordMapperPtr->To(Star.P, pt);

		if(pt.x <= 0 || pt.x >= AllottedGeometry.GetLocalSize().X)
		{
			continue;
		}

		const auto Size = Star.IsBig ? 4 : 2;
		const FVector2D S(Size, Size);
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);

		const auto Geometry = AllottedGeometry.MakeChild(S, Translation);

		float Tint = (float)(ABS(sin(Star.Age / Star.BlinkSpeed * PI)));
		Tint = Tint * 0.5f + 0.5f;

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(),
			&StarBrush,
			ESlateDrawEffect::None,
			Star.Color * Tint * InWidgetStyle.GetColorAndOpacityTint().A);
	}


	if(TerrainPtr != nullptr)
	{
		TerrainPtr->Draw(Painter, *CoordMapperPtr);
	}

	DrawObjects(Objects, Painter);
	DrawObjects(Enemies, Painter);
	DrawObjects(Debris,  Painter);
	DrawObjects(Blasts,  Painter);

	if(bShowBoundingBoxes && PlayerShipPtr && PlayerShipPtr->IsAlive())
	{
		DrawObjectBbox(PlayerShipPtr, Painter);
	}

	return LayerId;
}


void UDefconPlayMainWidgetBase::OnToggleDebugStats()
{
	PlayerShipDebug->PlayerShipPtr = PlayerShipPtr;
	PlayerShipDebug->Enemies       = Enemies;
	PlayerShipDebug->Debris        = Debris;
	PlayerShipDebug->ArenaHeight   = Daylon::GetWidgetSize(this).Y;

	const bool b = !PlayerShipDebug->IsVisible();
	
	PlayerShipDebug->SetVisibility(b ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}


void UDefconPlayMainWidgetBase::OnToggleShowBoundingBoxes()
{
	bShowBoundingBoxes = !bShowBoundingBoxes;

	FString Str = FString::Printf(TEXT("Bounding boxes %s"), bShowBoundingBoxes ? TEXT("ON") : TEXT("OFF"));
	AddMessage(Str);
}


void UDefconPlayMainWidgetBase::OnToggleShowOrigin()
{
	bShowOrigin = !bShowOrigin;
}


// ---------------------------------------------------------------------------------------------------

void UDefconPlayerShipDebugWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if(!IsVisible() || PlayerShipPtr == nullptr)
	{
		return;
	}

	auto Str = FString::Printf(TEXT("%d, %d"), (int32)PlayerShipPtr->Position.x, (int32)PlayerShipPtr->Position.y); 
	Position->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%d"), (int32)PlayerShipPtr->Orientation.Fwd.x); 
	OrientFwd->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->Velocity.x, PlayerShipPtr->Velocity.y); 
	Speed->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->ThrustVector.x, PlayerShipPtr->ThrustVector.y);
	Thrust->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Back].bActive, PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Back].Duration);
	ThrustLeftKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Fwd].bActive, PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Fwd].Duration);
	ThrustRightKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Up].bActive, PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Up].Duration);
	ThrustUpKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Down].bActive, PlayerShipPtr->NavControls[Defcon::ILiveGameObject::ENavControl::Down].Duration);
	ThrustDownKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->ThrustDurationForwards, PlayerShipPtr->ThrustDurationBackwards);
	Duration->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d%%"), ROUND(PlayerShipPtr->GetShieldStrength() * 100));
	ShieldLevel->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d"), Debris->Count());
	DebrisCount->SetText(FText::FromString(Str)); 


	if(TraceHiddenEnemies->IsChecked())
	{
		int32 Count = 0;
		Enemies->ForEach([&](Defcon::IGameObject* Object)
			{
				if(Object->Sprite && !Object->Sprite->IsVisible())
				{
					UE_LOG(LogGame, Error, TEXT("Enemy object %s is hidden"), *Defcon::GObjectTypeManager.GetName(Object->GetType()));
					Count++;
				}
			});
		Str = FString::Printf(TEXT("%d"), Count);
		HiddenEnemyCount->SetText(FText::FromString(Str));
	}
	else
	{
		HiddenEnemyCount->SetText(FText::FromString(TEXT("?")));
	}


	if(TraceOutOfBoundEnemies->IsChecked())
	{
		int32 Count = 0;

		Enemies->ForEach([&](Defcon::IGameObject* Object)
			{
				if(Object->Position.y < 0 || Object->Position.y > ArenaHeight || Object->Position.x < 0 || Object->Position.x > GArena->GetWidth())
				{
					UE_LOG(LogGame, Error, TEXT("Enemy object %s is out of bounds"), *Defcon::GObjectTypeManager.GetName(Object->GetType()));
					Count++;
				}
			});
		Str = FString::Printf(TEXT("%d"), Count);
		OutOfBoundEnemyCount->SetText(FText::FromString(Str));
	}
	else
	{
		OutOfBoundEnemyCount->SetText(FText::FromString(TEXT("?")));
	}


	Str = FString::Printf(TEXT("%d"), GDefconGameInstance->GetConstHumans().Count());
	HumansLeft->SetText(FText::FromString(Str));


	const auto Mission = GDefconGameInstance->GetMission();

	if(Mission == nullptr || !Mission->IsMilitary() || !Mission->IsRunning())
	{
		return;
	}

	const auto MilitaryMission = static_cast<Defcon::CMilitaryMission*>(Mission);

	if(Mission != nullptr && Mission->IsRunning())
	{
		Str = FString::Printf(TEXT("%d : %d"), MilitaryMission->TotalHostilesInPlay(), MilitaryMission->TargetsRemaining());
		EnemyCounts->SetText(FText::FromString(Str)); 
	}
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
