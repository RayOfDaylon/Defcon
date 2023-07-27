// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayMainWidgetBase.h"
#include "GameObjects/GameObjectCollection.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/Enemies/enemies.h"
#include "Common/PaintArguments.h"
#include "DefconUtils.h"
#include "DefconLogging.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

#pragma optimize("", off)

//const float StargateExpanderLifetime = 2.5f; // Same as FADE_DURATION_NORMAL in playviewbase.h

// todo: move to common/utils_core.h
// Function wrapper around check() macro so that other macros can call check() w/o expansion problems
static void Check(bool Condition) { check(Condition); }


void UDefconPlayMainWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// For the duration of the playviewbase, all playobject widgets will be installed/uninstalled on our canvas.

	Daylon::SetRootCanvas(RootCanvas);
	Daylon::SetWidgetTree(WidgetTree);

	//PlayerShipRightAtlas ->Atlas.InitCache();
	//PlayerShipLeftAtlas  ->Atlas.InitCache();

	// Load up the GameObjectResources global.

	GameObjectResources.SmartbombBrushPtr     = &SmartbombBrush;
	GameObjectResources.DebrisBrushRoundPtr   = &DebrisBrushRound;
	GameObjectResources.DebrisBrushSquarePtr  = &DebrisBrushSquare;

	// Our textures were 3x the arcade, but need to be 4.25x larger. So for now scale the widget sizes that much.
	// Note: player ship is already okay. Once we have our 4K textures the factor should be 0.5f.
	const auto UpscaleFactor = 4.25f / 3.0f;

#define ADD_ATLAS(_ObjType, _Atlas) \
	Check(_Atlas != nullptr); /* Did you forget to add Atlas asset to UUserWidget's texture list? */	\
	_Atlas->Atlas.InitCache();	\
	GameObjectResources.Add(Defcon::ObjType::_ObjType, { _Atlas, _Atlas->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f });

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

	bFirstTime = true;
}


void UDefconPlayMainWidgetBase::OnFinishActivating()
{
	Stars.Empty();
	Stars.Reserve(STARS_COUNT);

	for(int32 Idx = 0; Idx < STARS_COUNT; Idx++)
	{
		FStar Star;

		const float X = Daylon::FRandRange(0.0f, ArenaSize.X);

		const auto Elev = TerrainPtr->GetElev(X / ArenaSize.X) + 20;

		Star.P.set(X, Daylon::FRandRange(Elev, ArenaSize.Y));

		Stars.Add(Star);
	}

	PlayerShipExhaust =	Daylon::SpawnSpritePlayObject2D(PlayerShipExhaustAtlas->Atlas, FVector2D(5, 5), 0.5f);
	PlayerShipExhaust.Pin()->Hide();
}


void UDefconPlayMainWidgetBase::OnDeactivate()
{
	Daylon::Uninstall(PlayerShipExhaust.Pin());
}


void UDefconPlayMainWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if(CoordMapperPtr == nullptr || PlayerShipPtr == nullptr)
	{
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

	if(PlayerShipPtr->Sprite == nullptr)
	{
		return;
	}
	
	CFPoint pt;
	CoordMapperPtr->To(PlayerShipPtr->m_pos, pt);
	PlayerShipPtr->Sprite->SetPosition(FVector2D(pt.x, pt.y));
	PlayerShipPtr->Sprite->Update(DeltaTime);

	auto PlayerShipExhaustPtr = PlayerShipExhaust.Pin();

	if(PlayerShipExhaustPtr)
	{
		PlayerShipExhaustPtr->Update(DeltaTime);

		float ExhaustLength = 0.0f;

		if(PlayerShipPtr && PlayerShipPtr->IsAlive())
		{
			ExhaustLength =  ABS(3.0f * PlayerShipPtr->m_inertia.x);
		}

		if(ExhaustLength == 0.0f)
		{
			PlayerShipExhaustPtr->Hide();
		}
		else
		{
			const bool IsFacingRight = (PlayerShipPtr->m_orient.fwd.x > 0);
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


void UDefconPlayMainWidgetBase::DrawObjectBbox(Defcon::IGameObject* Object, FPaintArguments& FrameBuffer) const
{
	CFPoint pt;
	CoordMapperPtr->To(Object->m_pos, pt);

	CFRect r(pt);
	r.inflate(Object->m_bboxrad);

	if(r.UR.x > 0 && r.LL.x < FrameBuffer.AllottedGeometry->GetLocalSize().X)
	{
		FrameBuffer.ColorRect(r.LL.x, r.LL.y, r.UR.x, r.UR.y, C_RED);
	}
}


void UDefconPlayMainWidgetBase::DrawObjects(const Defcon::CGameObjectCollection* Collection, FPaintArguments& FrameBuffer) const
{
	if(Collection == nullptr)
	{
		return;
	}

	Collection->ForEach([&](Defcon::IGameObject* Object)
		{
			if(!Object->Sprite)
			{
				Object->Draw(FrameBuffer, *CoordMapperPtr);
			}

			if(bShowBoundingBoxes)
			{
				DrawObjectBbox(Object, FrameBuffer);
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

	FPaintArguments PaintArguments;

	auto PaintGeometry = AllottedGeometry.ToPaintGeometry();

	PaintArguments.PaintGeometry    = &PaintGeometry;
	PaintArguments.AllottedGeometry = &AllottedGeometry;
	PaintArguments.Args             = &Args;
	PaintArguments.LayerId          = LayerId;
	PaintArguments.MyCullingRect    = &MyCullingRect;
	PaintArguments.OutDrawElements  = &OutDrawElements;
	PaintArguments.RenderOpacity    = 1.0f;
	PaintArguments.InWidgetStyle    = &InWidgetStyle;


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
		TerrainPtr->Draw(PaintArguments, *CoordMapperPtr);
	}

	DrawObjects(Objects, PaintArguments);
	DrawObjects(Enemies, PaintArguments);
	DrawObjects(Debris,  PaintArguments);
	DrawObjects(Blasts,  PaintArguments);


	
#if 0
	// Draw any particles required by enemy materializations.
	Enemies->ForEach([&PaintArguments, this](Defcon::IGameObject* pObj)
		{
			auto EnemyPtr = static_cast<Defcon::CEnemy*>(pObj);

			if(EnemyPtr->IsMaterializing())
			{
				EnemyPtr->DrawMaterialization(PaintArguments, *CoordMapperPtr);
			}
		}
	);
#endif

	if(bShowBoundingBoxes && PlayerShipPtr && PlayerShipPtr->IsAlive())
	{
		DrawObjectBbox(PlayerShipPtr, PaintArguments);
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
	if(!this->IsVisible() || PlayerShipPtr == nullptr)
	{
		return;
	}

	auto Str = FString::Printf(TEXT("%d, %d"), (int32)PlayerShipPtr->m_pos.x, (int32)PlayerShipPtr->m_pos.y); 
	Position->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%d"), (int32)PlayerShipPtr->m_orient.fwd.x); 
	OrientFwd->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->m_velocity.x, PlayerShipPtr->m_velocity.y); 
	Speed->SetText(FText::FromString(Str));

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->m_thrustVector.x, PlayerShipPtr->m_thrustVector.y);
	Thrust->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlBack].bActive, PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlBack].fTimeDown);
	ThrustLeftKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlFwd].bActive, PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlFwd].fTimeDown);
	ThrustRightKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlUp].bActive, PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlUp].fTimeDown);
	ThrustUpKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%d, %.4f"), (int32)PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlDown].bActive, PlayerShipPtr->m_navCtls[Defcon::ILiveGameObject::ctlDown].fTimeDown);
	ThrustDownKey->SetText(FText::FromString(Str)); 

	Str = FString::Printf(TEXT("%.4f, %.4f"), PlayerShipPtr->m_fThrustDuration_Forwards, PlayerShipPtr->m_fThrustDuration_Backwards);
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
					UE_LOG(LogGame, Error, TEXT("Enemy object %s is hidden"), *Defcon::ObjectTypeManager.GetName(Object->GetType()));
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
				if(Object->m_pos.y < 0 || Object->m_pos.y > ArenaHeight)
				{
					UE_LOG(LogGame, Error, TEXT("Enemy object %s is out of bounds"), *Defcon::ObjectTypeManager.GetName(Object->GetType()));
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


	auto GI = gDefconGameInstance;// UDefconUtils::GetGameInstance(this);

	/*if(GI == nullptr)
	{
		return;
	}*/

	Str = FString::Printf(TEXT("%d"), GI->GetConstHumans().Count());
	HumansLeft->SetText(FText::FromString(Str));


	auto Mission = GI->GetMission();

	if(Mission == nullptr || !Mission->IsMilitary() || !Mission->IsRunning())
	{
		return;
	}

	auto MilitaryMission = static_cast<Defcon::CMilitaryMission*>(Mission);

	if(Mission != nullptr && Mission->IsRunning())
	{
		Str = FString::Printf(TEXT("%d : %d : %d"), Enemies->Count(), MilitaryMission->HostilesInPlay(), MilitaryMission->HostilesRemaining());
		EnemyCounts->SetText(FText::FromString(Str)); 
	}
}

#pragma optimize("", on)
