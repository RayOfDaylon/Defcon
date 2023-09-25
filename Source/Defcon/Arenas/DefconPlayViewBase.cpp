// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayViewBase.h"
#include "DefconGameInstance.h"
#include "DefconLogging.h"
#include "GameObjects/gameobjlive.h"
#include "GameObjects/human.h"
#include "GameObjects/Enemies/enemies.h"
#include "GameObjects/flak.h"
#include "GameObjects/bullet.h"
#include "GameObjects/smartbomb.h"
#include "GameObjects/mine.h"
#include "GameObjects/Auxiliary/materialization.h"
#include "Common/util_color.h"
#include "Common/util_math.h"
#include "Globals/MessageMediator.h"
#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "DaylonUtils.h"
//#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


// -----------------------------------------------------------------------------------------

#define MAX_RADARFRITZ	             3.0f
#define FADE_DURATION			     1.5f
#define FADE_DURATION_NORMAL	     0.25f // Controls how long to stay in arena while we teleport to next mission
#define PLAYERSHIP_VMARGIN          20.0f  // Keep player ship at least this far from top and bottom main widget edges


UDefconPlayViewBase* GArena = nullptr;



//float GameTime() { check(GArena != nullptr); return UKismetSystemLibrary::GetGameTimeInSeconds(GArena); }




void UDefconPlayViewBase::NativeOnInitialized()
{
	LOG_UWIDGET_FUNCTION
	Super::NativeOnInitialized();
}


void UDefconPlayViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);

	if(!IsInViewport())
	{
		return;
	}

	if(bDoneActivating && !IsPaused())
	{
		UpdateGameObjects(DeltaTime);

		if(Fader->IsVisible())
		{
			const float T = 1.0f - FMath::Max(0.0f, FadeAge / FADE_DURATION_NORMAL);

			Fader->SetRenderOpacity(T);
		}
	}
}


int32 UDefconPlayViewBase::NativePaint
(
	const FPaintArgs&        Args,
	const FGeometry&         AllottedGeometry,
	const FSlateRect&        MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32                    LayerId,
	const FWidgetStyle&      InWidgetStyle,
	bool                     bParentEnabled
) const
{
	LOG_UWIDGET_FUNCTION

	(void) Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);

/*	
	For the radar view edges, we need to draw them here	because at the bottom, 
	they overlap a red divider which is outside the radar widget. We need to 
	draw six half-gray lines, three on the top and three on the bottom, like this:

	  _____________________
	 |                     |

	 |_____________________|
*/

	if(/*RADAR_IS_PLAYER_CENTRIC ||*/ !IsValid(PlayAreaRadar))
	{
		return LayerId;
	}

	FPainter Painter;

	auto PaintGeom = AllottedGeometry.ToPaintGeometry();

	Painter.AllottedGeometry = &AllottedGeometry;
	Painter.Args             = &Args;
	Painter.bParentEnabled   = bParentEnabled;
	Painter.LayerId          = LayerId;
	Painter.MyCullingRect    = &MyCullingRect;
	Painter.OutDrawElements  = &OutDrawElements;
	Painter.RenderOpacity    = InWidgetStyle.GetColorAndOpacityTint().A;
	Painter.PaintGeometry    = &PaintGeom;


	const auto RadarSize = Daylon::GetWidgetSize(PlayAreaRadar);
	const auto RadarPos  = Daylon::GetWidgetPosition(PlayAreaRadar);

	const float T = 2.5f;
	const float B = RadarSize.Y + 2.5f; 

	CFPoint P, P2;
	MainAreaMapper.From(CFPoint(0, 0), P);
	PlayAreaRadar->GetCoordMapper().To(P, P2);

	const float L = P2.x + RadarPos.X;

	MainAreaMapper.From(CFPoint(MainAreaSize.X, 0), P);
	PlayAreaRadar->GetCoordMapper().To(P, P2);

	const float R = P2.x + RadarPos.X;

	Painter.DrawLine(L, T + 17, L, T - 2, C_DARK, 5.0f);
	Painter.DrawLine(R, T + 17, R, T - 2, C_DARK, 5.0f);
	Painter.DrawLine(L, T, R, T, C_DARK, 9.0f);
			
	Painter.DrawLine(L, B - 17, L, B + 2.0f, C_DARK, 5.0f);
	Painter.DrawLine(R, B - 17, R, B + 2.0f, C_DARK, 5.0f);
	Painter.DrawLine(L, B, R, B, C_DARK, 9.0f);

	return LayerId;
}


void UDefconPlayViewBase::OnMissionStarting()
{
	check(PlayAreaMain != nullptr);

	PlayAreaMain->OnMissionStarting();
}


Defcon::CGameObjectCollection& UDefconPlayViewBase::GetHumans()
{
	return Defcon::GGameMatch->GetHumans();
}


const Defcon::CGameObjectCollection& UDefconPlayViewBase::GetHumans() const
{
	return Defcon::GGameMatch->GetHumans();
}


void UDefconPlayViewBase::DeleteAllObjects()
{
	Enemies.  DeleteAll(Defcon::kIncludingSprites);
	Blasts.   DeleteAll(Defcon::kIncludingSprites);
	Debris.   DeleteAll(Defcon::kIncludingSprites);
	Objects.  DeleteAll(Defcon::kIncludingSprites);

	ScheduledTasks.DeleteAll();
}


#if 0
void UDefconPlayViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();
}
#endif


void UDefconPlayViewBase::OnFinishActivating()
{
	LOG_UWIDGET_FUNCTION
	Super::OnFinishActivating();

	check(Defcon::GGameMatch != nullptr);

	GArena = this;

	bIsPaused     = false;
	bBulletTime   = false;
	bArenaClosing = false;

	Daylon::Hide(Fader);

	ShipThrustSoundLoop = GAudio->CreateLoopedSound(Defcon::EAudioTrack::Playership_thrust);
	WasShipUnderThrust = false;

	InitCoordMappers();
	InitPlayerShipForMission();


	PlayAreaMain->Init(&GetHumans(), &Objects, &Enemies, &Debris, &Blasts, ArenaSize);
	PlayAreaMain->OnFinishActivating();

	if(!IsValid(GDefconGameInstance))
	{
		return;
	}


	// Place humans.

	GetHumans().ForEach([this](Defcon::IGameObject* Obj)
	{
		// Move each human down to avoid starting mission above terrain from potentially fatal height.

		auto Human = static_cast<Defcon::CHuman*>(Obj);

		Human->Position.y = FRANDRANGE(25, 30); // todo: use minimum terrain level pref instead of 25..30

		if(!Defcon::GGameMatch->GetHumansPlaced())
		{
			// We're in the first mission of the game, so randomize each human's x-position.
			Human->Position.x = FRANDRANGE(0.0f, GetWidth() - 1); 
		}

		Human->InitHuman(&GetObjects(), &GetEnemies());
	});


	Defcon::GGameMatch->SetHumansPlaced();


	// Install terrain -- must do this before mission is initialized.

	AllowableTerrainSpan.Set(MainAreaSize.Y * 0.05f, MainAreaSize.Y * 0.45f);
	CreateTerrain();


	// Start the current mission.
	// Causes player and human sprites to be installed.

	Defcon::GGameMatch->InitMission();

	AreHumansInMission = Defcon::GGameMatch->GetMission()->HumansInvolved();

	PlayAreaRadar->Init(MainAreaSize, (int32)ArenaWidth, &MainAreaMapper, &Objects, &Enemies);

	OnHumansChanged();


	PlayAreaMain  -> SetSafeToStart(); // todo: may not be needed
	PlayAreaRadar -> OnFinishActivating();

	bMissionDoneMsgShown = false;

	GetPlayerShip().EnableInput();

	GAudio->OutputSound(Defcon::EAudioTrack::Wave_start);
}


void UDefconPlayViewBase::OnDeactivate() 
{
	// We're being transitioned away from.
	// Empty all our object collections.

	LOG_UWIDGET_FUNCTION
	Super::OnDeactivate();

	PlayAreaMain ->OnDeactivate();
	PlayAreaRadar->OnDeactivate();

	// Because the object collections are owned by us and not by the current mission,
	// object deletions can happen after the current mission has been recreated 
	// due to the player ship getting destroyed, in which case it will attempt 
	// to remove hostiles in a new (and empty mission), borking the hostile count.
	// So for now, mark all enemies as non-mission-critical.

	Enemies.ForEach([](Defcon::IGameObject* Obj){ Obj->SetAsMissionTarget(false); });

	DeleteAllObjects();

	AllStopPlayerShip();
	GetPlayerShip().EnableInput(false);
}


bool UDefconPlayViewBase::IsOkayToFinishActivating() const
{
	auto const S = Daylon::GetWidgetSize(PlayAreaMain);

	return (S.X > 0.0f && S.Y > 0.0f);
}


void UDefconPlayViewBase::InitCoordMappers()
{
	MainAreaSize = Daylon::GetWidgetSize(PlayAreaMain);

	check(MainAreaSize.X > 0.0f && MainAreaSize.Y > 0.0f);

	ArenaSize = MainAreaSize * FVector2D(PLANET_TO_SCREEN_RATIO, 1.0);

	const auto MainS = Daylon::GetWidgetSize(PlayAreaMain);

	ArenaWidth = ArenaSize.X;
	MainAreaMapper.Init(MainS.X, MainS.Y, ArenaWidth);
	PlayAreaMain->CoordMapperPtr = &MainAreaMapper;

	MainAreaStarsMapper.Init(MainS.X, MainS.Y, ArenaWidth / 2);
	PlayAreaMain->CoordMapperStarsPtr = &MainAreaStarsMapper;
}


void UDefconPlayViewBase::CreateTerrain()
{
	SAFE_DELETE(Terrain);

	if(Defcon::GGameMatch->GetMission()->UsesTerrain())
	{
		Terrain = new Defcon::CTerrain;
		Terrain->InitTerrain(GetWidth(), GetHeight(), AllowableTerrainSpan);
	}

	PlayAreaMain ->SetTerrain(Terrain);
	PlayAreaRadar->SetTerrain(Terrain);
}


void UDefconPlayViewBase::InitPlayerShipForMission()
{
	// Set up the player ship for the start of a mission.

	LOG_UWIDGET_FUNCTION
	auto& PlayerShip = GetPlayerShip();

	PlayerShip.MapperPtr = &MainAreaMapper;
	PlayerShip.InitPlayerShip();

	// Position the ship at x=0, and halfway up.
	PlayerShip.Position.Set(0.0f, ArenaSize.Y / 2);
	PlayerShip.FaceRight();
	
	// Sprite installation for player ship happens in DefconPlayMainWidgetBase
	PlayerShip.SetIsAlive(true);

	PlayerShip.Orientation.Fwd.x = 1.0f;

	AllStopPlayerShip();
	PlayerShip.EnableInput(false);

	NumPlayerPassengers = 0;

	PlayerShip.SetShieldStrength(1.0f, true);

	PlayerShip.AddSmartBombs(0); // hack: get readout to size properly
}


void UDefconPlayViewBase::TransportPlayerShip()
{
	// The player ship entered the stargate but the mission isn't over.
	// If any humans are being abducted, place the ship at the highest one
	// and maintain the ship's direction.

	CFPoint P(0.0f, 0.0f);

	if(AreHumansInMission)
	{
		GetHumans().ForEach([&](Defcon::IGameObject* Object)
		{
			auto Human = static_cast<Defcon::CHuman*>(Object);

			if(Human->IsBeingAbducted())
			{
				// Track highest human but not one that is too close to the top of the arena.
				if(Human->Position.y > P.y && Human->Position.y < ArenaSize.Y - 150)
				{
					P = Human->Position;
				}
			}
		});
	}

	auto& PlayerShip = GetPlayerShip();

	const auto Mission = static_cast<Defcon::CMilitaryMission*>(Defcon::GGameMatch->GetMission());

	if(P.y != 0.0f)
	{
		// A human is being abducted.

		// Move player to where human is, plus some height so the lander can be targeted.

		PlayerShip.Position = P;
		PlayerShip.Position.y += 100;

		// Move player horizontally by half the screen width, but facing the human.
		// If the player is still inside the stargate, adjust the distance until he's clear.
		float DistanceFactor = 0.4f;

		do
		{
			DistanceFactor += 0.1f;

			PlayerShip.Position.x = WrapX(P.x - (PlayerShip.Orientation.Fwd.x * GetDisplayWidth() * DistanceFactor));

		} while(Mission->PlayerInStargate());
	}
	else
	{
		// No humans are being abducted. 
		// Apparently the arcade game teleported the ship to the other side of the planet,
		// which in our case is the x-origin.
#if 1
		PlayerShip.Position.x = 0.0f;
#else
		// We used to teleport the player randomly.
		do
		{
			PlayerShip.Position.Set(WrapX(FRAND * ArenaWidth), (FRAND * 0.8f + 0.1f) * ArenaSize.Y);
		} while(Mission->PlayerInStargate());
#endif
	}

	// The KeepPlayerShipInView routine assumes the player ship is visible, and when it's not, 
	// it just slowly pans the arena until it is. We have to pan the arena immediately 
	// so that KeepPlayerShipInView doesn't have to do anything.

	// Get the player ship's screen location. If it's not at the margin (and it almost certainly won't be)
	// then we need to pan by the difference.

	CFPoint ScreenPt;
	MainAreaMapper.To(PlayerShip.Position, ScreenPt);

	const float kMargin = PLAYER_POSMARGIN;

	float DeltaX;

	if(PlayerShip.Orientation.Fwd.x > 0)
	{
		// Player facing right.
		DeltaX = ScreenPt.x - kMargin;
	}
	else
	{
		// Player facing left.
		DeltaX = (GetDisplayWidth() - kMargin - ScreenPt.x) * -1;
	}

	MainAreaMapper.SlideBy(DeltaX);
	MainAreaStarsMapper.SlideBy(DeltaX * 0.5f);
}


void UDefconPlayViewBase::KeepPlayerShipInView(float DeltaTime)
{
	// Ensure that the main arena widget keeps the player in view.
	// We not only pan the arena mapper, we also keep the player ship 
	// within some fixed margin of the screen edge being pointed away from.
	// That way, if the user reverses direction and does nothing else, 
	// the screen will quietly pan to put most of the arena in front of him.

	// rcg, jun 24/04: a ship with a larger maxthrust 
	// is able to outrun the margin settler. Increasing 
	// the player_marginsettlespeed fixes it, but 
	// then the ship snaps too quickly when the 
	// thrust is no longer applied. What we need to do
	// is scale the settler with the player's speed.


	const auto& PlayerShip = GetPlayerShip();

	const float kMargin   = PLAYER_POSMARGIN;
	const float kPanSpeed = PLAYER_MARGINSETTLESPEED * (1.0f + FMath::Abs(PlayerShip.GetVelocity().x) / (PLAYER_MARGINSETTLESPEED * 1.5f));
	const float kPanAvail = kPanSpeed * DeltaTime;

	CFPoint ScreenPt;
	MainAreaMapper.To(PlayerShip.Position, ScreenPt);

	float DeltaX;

	if(PlayerShip.Orientation.Fwd.x > 0)
	{
		// Player facing right.
		DeltaX = FMath::Min(kPanAvail, ScreenPt.x - kMargin);
	}
	else
	{
		// Player facing left.
		DeltaX = FMath::Min(kPanAvail, GetDisplayWidth() - kMargin - ScreenPt.x) * -1;
	}

	MainAreaMapper.SlideBy(DeltaX);
	MainAreaStarsMapper.SlideBy(DeltaX * 0.5f);


	if(!RADAR_IS_PLAYER_CENTRIC)
	{
		// Update the radar coord mapper.
	
		CFPoint L;
		MainAreaMapper.From(CFPoint(0, 0), L);

		const float VVC = WrapX(L.x + MainAreaSize.X / 2 - ArenaWidth / 2);

		PlayAreaRadar->GetCoordMapper().ScrollTo(CFPoint(VVC, 0.0f));
	}
}


void UDefconPlayViewBase::AllStopPlayerShip()
{
	auto& PlayerShip = GetPlayerShip();

	PlayerShip.ZeroInput();
	PlayerShip.ZeroMotion();
	PlayerShip.ZeroThrust();
	PlayerShip.ZeroVelocity();
}


void UDefconPlayViewBase::IncreaseScore(int32 Points, bool bVis, const CFPoint* pPos)
{
	Defcon::GGameMatch->AdvanceScore((int32)Points);
}


float UDefconPlayViewBase::GetTerrainElev(float X) const
{
	check(X >= 0 && X <= GetWidth());

	return Terrain->GetElev(X / GetWidth());
}


void UDefconPlayViewBase::ConcludeMission()
{
	// Wave can end. todo: handle via game match?

	if(!bArenaClosing)
	{
		bArenaClosing = true;
		auto Task = new Defcon::CEndMissionTask;
		Task->Countdown = FADE_DURATION_NORMAL;
		ScheduledTasks.Add(Task);

		FadeAge = FADE_DURATION_NORMAL;

		Daylon::Show(Fader);
	}
}


void UDefconPlayViewBase::DestroyPlayerShip()
{
	auto& PlayerShip = GetPlayerShip();

	AllStopPlayerShip();
	PlayerShip.EnableInput(false);
	PlayerShip.Lifespan = 0.0f;
	PlayerShip.SetIsAlive(false);

	// Start the player ship destruction sequence.
	// We do this with a special game object.

	auto DestroyedShip = new Defcon::CDestroyedPlayerShip();

	DestroyedShip->InitDestroyedPlayer(PlayerShip.Position, PlayerShip.BboxRadius, DESTROYED_PLAYER_PARTICLE_SPEED, DESTROYED_PLAYER_PARTICLE_MIN_LIFETIME, DESTROYED_PLAYER_PARTICLE_MAX_LIFETIME);

	DestroyedShip->Position        = PlayerShip.Position;
	DestroyedShip->Orientation.Fwd = PlayerShip.Orientation.Fwd;

	DestroyedShip->Sprite->FlipHorizontal = (DestroyedShip->Orientation.Fwd.x < 0);

	DestroyedShip->InstallSprite();

	Objects.Add(DestroyedShip);

	GAudio->OutputSound(Defcon::EAudioTrack::Player_dying);

	PlayerShip.OnAboutToDie();

	GDefconGameInstance->GetStats().PlayerDeaths++;

	// Deassign target for all enemies so that they don't keep moving towards and firing at the dying (and soon nonexistant) player.
	Enemies.ForEach([](Defcon::IGameObject* Obj){ static_cast<Defcon::CEnemy*>(Obj)->SetTarget(nullptr); });
}


void UDefconPlayViewBase::CheckIfPlayerHit(Defcon::CGameObjectCollection& objects)
{
	auto& PlayerShip = GetPlayerShip();

	if(!PlayerShip.IsAlive() || !PlayerShip.CanBeInjured())
	{
		return;
	}

	CFRect bbox;
	CFPoint injurePt;

	bbox.Set(PlayerShip.Position);
	bbox.Inflate(PlayerShip.BboxRadius);


	Defcon::IGameObject* pObj = Objects.GetFirst();

	while(pObj != nullptr)
	{
		if(pObj->IsInjurious() && pObj->GetCreatorType() != Defcon::EObjType::PLAYER)
		{
			pObj->GetInjurePt(injurePt);

			const bool bHit = (bbox.PtInside(injurePt) || pObj->TestInjury(bbox));

			if(bHit)
			{
				// PlayerShip ship has been hit by a bullet.

				GDefconGameInstance->GetStats().PlayerHits++;

				const bool bPlayerKilled = !Defcon::GGameMatch->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce() * PLAYERSHIP_IMPACT_SCALE);

				if(bPlayerKilled)
				{
					DestroyPlayerShip();
				}
				else
				{
					ShieldBonk(&PlayerShip, pObj->GetCollisionForce());
				}

				pObj->MarkAsDead();

				if(bPlayerKilled)
				{
					break;
				}
			}
		}
		pObj = pObj->GetNext();
	}
}


void UDefconPlayViewBase::CheckPlayerCollided()
{
	// See if the player collided with an enemy.

	auto& PlayerShip = GetPlayerShip();


	if(!PlayerShip.IsAlive() || !PlayerShip.CanBeInjured())
	{
		return;
	}

	CFRect rObj, rPlayer(PlayerShip.Position);
	rPlayer.Inflate(PlayerShip.BboxRadius);
	
	Defcon::IGameObject* pObj = Enemies.GetFirst();

	while(pObj != nullptr)
	{
		if(pObj->IsCollisionInjurious())
		{
			rObj.Set(pObj->Position);
			rObj.Inflate(pObj->BboxRadius);

			if(rObj.Intersect(rPlayer))
			{
				GDefconGameInstance->GetStats().PlayerCollisions++;

				const bool bPlayerKilled = !Defcon::GGameMatch->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce() * PLAYERSHIP_IMPACT_SCALE);

				if(bPlayerKilled)
				{
					DestroyPlayerShip();
				}
				else
				{
					ShieldBonk(&PlayerShip, pObj->GetCollisionForce());
				}

				// Process collision outcome for object.
				if(pObj->CanBeInjured())
				{
					DestroyObject(pObj, !bPlayerKilled);
				}

				if(bPlayerKilled)
				{
					break;
				}
			}			
		}
		pObj = pObj->GetNext();
	}
}


void UDefconPlayViewBase::AddDebris(Defcon::IGameObject* p)
{
	Debris.Add(p);
}


Defcon::CPlayerShip& UDefconPlayViewBase::GetPlayerShip() 
{
	return Defcon::GGameMatch->GetPlayerShip();
}


const Defcon::CPlayerShip& UDefconPlayViewBase::GetPlayerShip() const
{
	return Defcon::GGameMatch->GetPlayerShip();
}


void UDefconPlayViewBase::UpdateGameObjects(float DeltaTime)
{
	if(!bArenaClosing)
	{
		if(!Defcon::GGameMatch->Update(DeltaTime))
		{
			ConcludeMission();
		}
	}

	ScheduledTasks.Process(DeltaTime);


	if(ARENA_BOGDOWN && Debris.Count() >= 500)
	{
		// Debris count over 700 can occur from smartbomb exploding many enemies.
		// Arena bogdown lets us slow down the action to make it more intense.

		float Factor = NORM_((float)Debris.Count(), 500.0f, 1500.0f);
		Factor = CLAMP(Factor, 0.0f, 1.0f);

		DeltaTime *= LERP(1.0f, 0.67f, Factor);
	}

	if(bBulletTime)
	{
		DeltaTime *= 0.25f;
	}
	

	if(FadeAge > 0.0f)
	{
		FadeAge -= DeltaTime;
	}

	ProcessWeaponsHits();

	CheckPlayerCollided();
	CheckIfPlayerHit(Objects);

	// Move and draw everyone.


	const auto Mission = Defcon::GGameMatch->GetMission();

	// Mission becomes nullptr at end of game, so check for that.
	
	const bool HumansInvolved = Mission != nullptr ? Mission->HumansInvolved() : false;

	// ------------------------------------------------------
	// Process objects (move, draw, delete).

	static int32 n = 0;
	const bool bDrawRadar = (n++ % 5 == 0);

	// Note: update only model data, not visual.
	Defcon::GameObjectProcessingParams GOP;

	GOP.UninstallSpriteIfObjectDeleted = true;
	GOP.DeltaTime	                   = DeltaTime;
	GOP.MapperPtr                      = &MainAreaMapper;

	Blasts.Process(GOP);
	Debris.Process(GOP);


	if(HumansInvolved)
	{
		const auto NumHumansBefore = GetHumans().Count();
		GetHumans().Process(GOP);
		const auto NumHumansNow = GetHumans().Count();

		if(NumHumansNow < NumHumansBefore)
		{
			OnHumansChanged();

			FString Str;

			if(NumHumansNow > 0)
			{
				Str = FString::Printf(TEXT("HUMANS REMAINING: %d"), NumHumansNow);
			}
			else
			{
				Str = TEXT("ALL HUMANS ABDUCTED OR KILLED");
			}

			Defcon::GMessageMediator.TellUser(Str, 3.0f);

		}
	}

	
	Objects.Process(GOP);


	// Extra stuff when processing enemies.

	if(Mission != nullptr && Mission->IsMilitary())
	{
		const auto MilitaryMission = static_cast<Defcon::CMilitaryMission*>(Mission);

		GOP.OnDeath = [this, MilitaryMission](Defcon::IGameObject* ObjPtr)
		{
			if(ObjPtr->GetType() == Defcon::EObjType::LANDER)
			{
				if(MilitaryMission->LandersRemaining() == 1)
				{
					FString Str = TEXT("ALL LANDERS DESTROYED");
					Defcon::GMessageMediator.TellUser(Str, MESSAGE_DURATION_IMPORTANT);
				}
			}
		};

		if(!bMissionDoneMsgShown)
		{
			GOP.OnPostDeath = [this, MilitaryMission]()
			{
				if(MilitaryMission->IsMissionComplete())
				{
					FText Text = FText::FromString(TEXT("ALL MISSION TARGETS DESTROYED -- PROCEED TO STARGATE"));
					Defcon::GMessageMediator.Send(Defcon::EMessageEx::SetTopMessage, &Text);
					bMissionDoneMsgShown = true;
				}
			};
		}
	}

	Enemies.Process(GOP);

	// ------------------------------------------------------

	auto& PlayerShip = GetPlayerShip();

	if(PlayerShip.IsAlive())
	{
		PlayerShip.Tick(DeltaTime);

		PlayerShip.Position.x = WrapX(PlayerShip.Position.x);

		// Constrain player vertically.
		PlayerShip.Position.y = CLAMP(PlayerShip.Position.y, PLAYERSHIP_VMARGIN, ArenaSize.Y - PLAYERSHIP_VMARGIN);

		KeepPlayerShipInView(DeltaTime);
		DoThrustSound(DeltaTime);


		if(HumansInvolved)
		{
			// See if any humans can be debarked.
			if(PlayerShip.Position.y < 5.0f + GetTerrainElev(PlayerShip.Position.x))
			{
				if(NumPlayerPassengers > 0 && PlayerShip.DebarkOnePassenger(GetHumans()))
				{
					NumPlayerPassengers--;
					IncreaseScore(HUMAN_VALUE_DEBARKED, true, &PlayerShip.Position);
				}
			}
			else
			{
				// See if any falling humans can be carried.

				const CFPoint playerPos = PlayerShip.Position;

				CFRect rPlayer(playerPos);
				rPlayer.Inflate(PlayerShip.GetPickupRadiusBox());

				GetHumans().ForEachUntil([&](Defcon::IGameObject* Obj)
				{
					auto Human = static_cast<Defcon::CHuman*>(Obj);

					const CFPoint humanPos = Obj->Position;

					if(!(Human->IsFalling() && rPlayer.Intersect(CFRect(humanPos))))
					{
						return true;
					}
					
					if(PlayerShip.EmbarkPassenger(Obj, GetHumans()))
					{
						NumPlayerPassengers++;
						IncreaseScore(HUMAN_VALUE_EMBARKED, true, &PlayerShip.Position);
					}
					return false;
				});
			}
		}
	}
	else
	{
		// The player ship is toast; restart mission.

		OnPlayerShipDestroyed();
	}

	// todo: If our shields are taking a beating, fritz the radar.
	if(m_fRadarFritzed > 0.0f)
	{
		//m_virtualRadarScreen.DarkenRect(0,0,10000, 10000, 1.0f - (m_fRadarFritzed / MAX_RADARFRITZ));
		m_fRadarFritzed -= DeltaTime;
	}
}


void UDefconPlayViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MissionPicker);
}


void UDefconPlayViewBase::OnPausePressed()
{
	bIsPaused = !bIsPaused;

	FText Text = FText::FromString(bIsPaused ? TEXT("GAME PAUSED") : TEXT(""));

	Defcon::GMessageMediator.Send(Defcon::EMessageEx::SetTopMessage, &Text);
}


void UDefconPlayViewBase::OnBulletTimePressed()
{
	bBulletTime = !bBulletTime;

	FString Str = (bBulletTime ? TEXT("BULLET TIME ON") : TEXT("BULLET TIME OFF"));
	Defcon::GMessageMediator.TellUser(Str);
}


void UDefconPlayViewBase::OnToggleDebugStats()
{
	PlayAreaMain->OnToggleDebugStats();
}


void UDefconPlayViewBase::OnToggleShowBoundingBoxes() 
{
	PlayAreaMain->OnToggleShowBoundingBoxes();
}


void UDefconPlayViewBase::OnToggleShowOrigin() 
{
	PlayAreaMain->OnToggleShowOrigin();
}


void UDefconPlayViewBase::DoThrustSound(float DeltaTime)
{
	const bool bThrustStateChanged = (WasShipUnderThrust != IsPlayerShipThrustActive());

	WasShipUnderThrust = IsPlayerShipThrustActive();

	if(IsPlayerShipThrustActive())
	{
		if(bThrustStateChanged)
		{
			ShipThrustSoundLoop.Start();
		}
		else
		{
			// Still thrusting.
			ShipThrustSoundLoop.Tick(DeltaTime);
		}
	}
}


bool UDefconPlayViewBase::IsPlayerShipThrustActive() const
{
	const auto& PlayerShip = GetPlayerShip();

	return (PlayerShip.GetNavControl(Defcon::ILiveGameObject::ENavControl::Fwd).bActive || 
	        PlayerShip.GetNavControl(Defcon::ILiveGameObject::ENavControl::Back).bActive);
}


void UDefconPlayViewBase::OnPawnNavEvent(EDefconPawnNavigationEvent Event, bool Active)
{
	// Move the player ship. todo: could be done in gameinstance.

	auto& PlayerShip = GetPlayerShip();

	if(!(PlayerShip.IsAlive() && PlayerShip.IsInputEnabled()))
	{
		return;
	}

	int32 NavCtlIndex = -1;

	switch(Event)
	{
		case EDefconPawnNavigationEvent::Up:    NavCtlIndex = Defcon::ILiveGameObject::ENavControl::Up;    break;
		case EDefconPawnNavigationEvent::Down:  NavCtlIndex = Defcon::ILiveGameObject::ENavControl::Down;  break;
		case EDefconPawnNavigationEvent::Left:  NavCtlIndex = Defcon::ILiveGameObject::ENavControl::Back;  PlayerShip.FaceLeft();  break;
		case EDefconPawnNavigationEvent::Right: NavCtlIndex = Defcon::ILiveGameObject::ENavControl::Fwd;   PlayerShip.FaceRight(); break;
		case EDefconPawnNavigationEvent::Hyperspace: Hyperspace(); return;
	
		default:
			return;
	}

	const auto& CurrentNavCtl = PlayerShip.GetNavControl(NavCtlIndex);

	const bool EventBecameActive = !CurrentNavCtl.bActive && Active;

	PlayerShip.SetNavControl(NavCtlIndex, Active, CurrentNavCtl.Duration);

	if(EventBecameActive)
	{
		PlayerShip.SetNavControl(NavCtlIndex, true, 0.0f);
	}
}


void UDefconPlayViewBase::OnNavEvent(ENavigationKey Key)
{
	// debugging only
	const float Direction = (Key == ENavigationKey::Left ? -1.0f : 1.0f);

	CFPoint pt = MainAreaMapper.GetOffset();
	pt.x += Direction * 100.0f;
	pt.x = WrapX(pt.x);// FMath::Wrap(pt.x, 0.0f, MainAreaMapper.PlanetCircumference);
	//pt.x = (int32)(pt.x + MainAreaMapper.PlanetCircumference) % (int32)MainAreaMapper.PlanetCircumference;
	MainAreaMapper.ScrollTo(pt);

	// Move the player to keep pace.
	//PlayerShip.Position.x = FMath::Wrap(pt.x + 1920/2, 0.0f, MainAreaMapper.PlanetCircumference);
}


void UDefconPlayViewBase::OnPawnWeaponEvent(EDefconPawnWeaponEvent Event, bool Active)
{
	switch(Event)
	{
		case EDefconPawnWeaponEvent::FireLaser: 

			if(GetPlayerShip().IsAlive())
			{
				GetPlayerShip().FireLaserWeapon(Objects);

				if(GetPlayerShip().AreDoubleGunsActive())
				{
					GDefconGameInstance->GetStats().DoubleShotsFired++;
				}
				else
				{
					GDefconGameInstance->GetStats().ShotsFired++;
				}

				if(true/*BRAND*/)
					GAudio->OutputSound(Defcon::EAudioTrack::Laserfire);
				else
					GAudio->OutputSound(Defcon::EAudioTrack::Laserfire_alt);
			}
			break;


		case EDefconPawnWeaponEvent::DetonateSmartbomb: 

			if(GetPlayerShip().IsAlive())
			{
				if(GetPlayerShip().AcquireSmartBomb())
				{
					DetonateSmartbomb();
				}
				else
				{
					FString Str = TEXT("SMARTBOMB ORDNANCE DEPLETED");
					Defcon::GMessageMediator.TellUser(Str);

					GAudio->OutputSound(Defcon::EAudioTrack::Invalid_selection);
				}
			}
			break;


		case EDefconPawnWeaponEvent::ToggleDoubleGuns:
			{
				GetPlayerShip().ToggleDoubleGuns();
				FString Str = FString::Printf(TEXT("DOUBLE LASER CANNONS %sACTIVATED"), GetPlayerShip().AreDoubleGunsActive() ? TEXT("") : TEXT("DE"));
				Defcon::GMessageMediator.TellUser(Str);
			}
			break;
	}
}


bool UDefconPlayViewBase::IsPointVisible(const CFPoint& WorldPos) const
{
	// Given an arena location, convert to display space and see if within bounds.

	CFPoint DisplayPos;
	MainAreaMapper.To(WorldPos, DisplayPos);

	return (DisplayPos.x >= 0 && DisplayPos.x < GetDisplayWidth());
}


typedef enum EFiringAccuracy { Wild, At, Lead };


float UDefconPlayViewBase::ShortestDirection(const CFPoint& WorldPosA, const CFPoint& WorldPosB, CFPoint& Result) const
{
	// Return direction to achieve shortest travel time 
	// from point A to point B. The distance is also returned.

	const float W = GetWidth();

	const float FromTail1 = W - WorldPosA.x;
	const float FromTail2 = W - WorldPosB.x;

	const float D0 = ABS(WorldPosB.x - WorldPosA.x);
	const float D1 = WorldPosA.x + FromTail2;
	const float D2 = WorldPosB.x + FromTail1;

	const float HorzDistance = FMath::Min(D0, FMath::Min(D1, D2));

	check(HorzDistance <= W / 2);

	if(D0 == HorzDistance)
	{
		Result = WorldPosB - WorldPosA;
	}
	else if(D1 == HorzDistance)
	{
		// B behind origin, A past it.
		Result = WorldPosB;
		Result.x -= W;
		Result -= WorldPosA;
	}
	else if(D2 == HorzDistance)
	{
		// A behind origin, B past it.
		Result = WorldPosA;
		Result.x -= W;
		Result = WorldPosB - Result;
	}

	const float Distance = Result.Length();
	check(ABS(Result.x) <= W / 2);

	Result.Normalize();

	return Distance;
}


void UDefconPlayViewBase::Lerp(const CFPoint& WorldPosA, const CFPoint& WorldPosB, CFPoint& Result, float T) const
{
	// Lerp arena coords A and B.
	// Lerp locations lie along the shortest line between A and B.

	CFPoint Dir;
	const float Len = ShortestDirection(WorldPosA, WorldPosB, Dir);

	Dir *= Len * T;
	Result = WorldPosA + Dir;

	Result.x = WrapX(Result.x);
}


float UDefconPlayViewBase::WrapX(float WorldX) const
{
	const auto W = GetWidth();

	if(WorldX > W) return WorldX - W;
	if(WorldX < 0) return WorldX + W;

	return WorldX;
}


void UDefconPlayViewBase::LayMine(Defcon::IGameObject& Obj, const CFPoint& from, int32, int32)
{
	auto Mine = new Defcon::CMine;

	Mine->InstallSprite();
	Mine->Position = Obj.Position - Obj.Orientation.Fwd * (2 * FMath::Max(Obj.BboxRadius.x, Obj.BboxRadius.y));

	Objects.Add(Mine);
}


Defcon::IBullet* UDefconPlayViewBase::FireBullet(Defcon::IGameObject& Obj, const CFPoint& From, int32 SoundID, int32)
{
	Defcon::IBullet* Bullet = Obj.MakeBullet();

	check(Bullet != nullptr);
		
	Bullet->SetCreatorType(Obj.GetType());
	Bullet->InstallSprite();

	// Scores at which to switch firing style.
	const int32 FIRE_AT   = 10000;
	const int32 FIRE_LEAD = 100000;

	const int32 Score = Defcon::GGameMatch->GetScore();

	EFiringAccuracy eAccuracy = EFiringAccuracy::Lead;

	if(Score < FIRE_AT)
	{
		eAccuracy = EFiringAccuracy::Wild;
	}
	else if(Score < FIRE_LEAD)
	{
		// Between firing at and leading.
		const float F = NORM_(Score, FIRE_AT, FIRE_LEAD);
		eAccuracy = FRAND < F ? EFiringAccuracy::Lead : EFiringAccuracy::At;
	}

	CFPoint Direction;
	ShortestDirection(From, GetPlayerShip().Position, Direction);


	switch(eAccuracy)
	{
		case EFiringAccuracy::Wild:
			// Bullet is fired some random angle away from the player.
		{
			// firing angle is +/- 45 deg to zero
			// depending on player score.
			Direction.Rotate(SFRAND * MAP(Score, 0, FIRE_AT, 45, 0));
		}
			break;

		case EFiringAccuracy::At:
			// Bullet is fired at player's current position.
			break;

		case EFiringAccuracy::Lead:
			// Lead the shot, taking player's velocity 
			// into account. Project where the player 
			// will be in some time from now, and 
			// compute a vector and speed to that spot.

			const float Time = FRANDRANGE(0.75f, 1.25f);
			CFPoint P = GetPlayerShip().Position + GetPlayerShip().GetVelocity() * Time;

			P.x = WrapX(P.x);

			Bullet->SetSpeed((1.0f / Time) * ShortestDirection(From, P, Direction));
			break;
	}

	Bullet->Orientation.Fwd = Direction;
	// Place bullet just outside shooter, otherwise 
	// shooter will shoot himself. We could also solve 
	// this by making the shooter (or all members of 
	// the shooter's class) impervious to the bullet, 
	// but that would deny interesting friendly fire.
	Bullet->Position = From + Bullet->Orientation.Fwd * (2 * FMath::Max(Obj.BboxRadius.x, Obj.BboxRadius.y));

	GAudio->OutputSound((Defcon::EAudioTrack)(SoundID - 1 + (int32)Defcon::EAudioTrack::Bullet));

	Objects.Add(Bullet);

	return Bullet;
}


float UDefconPlayViewBase::Xdistance(float WorldX1, float WorldX2) const
{
	// Return the shortest distance between two 
	// horizontal locations on the arena.
	
	const float FromTail1 = ArenaWidth - WorldX1;
	const float FromTail2 = ArenaWidth - WorldX2;

	const float D0 = ABS(WorldX2 - WorldX1);
	const float D1 = WorldX1 + FromTail2;
	const float D2 = WorldX2 + FromTail1;

	return FMath::Min(D0, FMath::Min(D1, D2));
}


Defcon::CHuman* UDefconPlayViewBase::FindNearestHuman(float WorldX) const
{
	// Return the uncarried humanoid nearest to a given point.

	Defcon::CHuman* Result = nullptr;

	GetHumans().ForEachUntil([this, &Result, &WorldX](Defcon::IGameObject* Obj)
	{
		auto Human = static_cast<Defcon::CHuman*>(Obj);

		if(!Human->IsBeingCarried() && Human->Lifespan > 0.1f)
		{
			const float Distance = Xdistance(Human->Position.x, WorldX);

			if(Distance < HUMAN_WITHINRANGE && Human->Position.y < GetTerrainElev(Human->Position.x))
			{
				Result = Human;
				return false;
			}
		}

		return true;
	});

	return Result;
}


void UDefconPlayViewBase::DestroyObject(Defcon::IGameObject* pObj, bool bExplode)
{
	check(pObj != nullptr);

	// If object is already dying, then don't kill it again. 
	// This fixes the problem where player got rewarded for 
	// killing the same enemy multiple times.

	if(pObj->MarkedForDeath())
	{
		return;
	}

	pObj->MarkAsDead();
	pObj->OnAboutToDie();

	if(pObj->GetType() != Defcon::EObjType::PLAYER)
	{
		const auto Mission = Defcon::GGameMatch->GetMission();

		if(Mission != nullptr && Mission->IsMilitary())
		{
			IncreaseScore(pObj->GetPointValue(), SCORETIPS_SHOWENEMYPOINTS, &pObj->Position);
		}
	}

	if(bExplode)
	{
		pObj->Explode(Debris);

		const float ExplosionMass = pObj->GetExplosionMass();
		
		Defcon::EAudioTrack Track;

		if(ExplosionMass >= 1.0f) 
		{
			const Defcon::EAudioTrack LargeExplosionSounds[] =
			{
				Defcon::EAudioTrack::Ship_exploding,
				Defcon::EAudioTrack::Ship_exploding2,
				Defcon::EAudioTrack::Ship_exploding2a,
				Defcon::EAudioTrack::Ship_exploding2b
			};

			Track = LargeExplosionSounds[IRAND(array_size(LargeExplosionSounds))] ;
		}
		else if(ExplosionMass >= 0.3f)
		{
			Track = Defcon::EAudioTrack::Ship_exploding_medium;
		}
		else
		{
			Track = Defcon::EAudioTrack::Ship_exploding_small;
		}

		GAudio->OutputSound(Track);
	}
}


void UDefconPlayViewBase::CheckIfObjectsGotHit(Defcon::CGameObjectCollection& ObjectsToCheck)
{
	// See if anything got hit, and if so, 
	// mark them for injury or death.

	// Cross ref all injurious objects (weapons fire)
	// with all injurable objects (such as player and enemies).


	CFRect  Bbox;
	CFPoint InjurePt;
	CFPoint ScreenPos;

	// The debris is a seperate game object collection
	// so that we can build it up without disturbing 
	// the arena's object collection, which is necessary 
	// since we are searching through it.
	//CGameObjectCollection	debris;
	const int32 DisplayWidth = GetDisplayWidth();

	// Loop through the weapons fire, 
	// then, for each bullet, loop through the targets.

	Objects.ForEach([&](Defcon::IGameObject* Obj)
	{
		if(!Obj->IsInjurious())
		{
			return;
		}
		
		Obj->GetInjurePt(InjurePt);

		Defcon::IGameObject* Obj2 = ObjectsToCheck.GetFirst();

		while(Obj2 != nullptr)
		{
			if(!Obj2->CanBeInjured())
			{
				Obj2 = Obj2->GetNext();
				continue;
			}

			if((Obj->GetParentType() == Defcon::EObjType::BULLET) && ENEMIES_CANTHURTEACHOTHER)
			{
				// For non-players, bullets don't hurt.
				Obj2 = Obj2->GetNext();
				continue;
			}
			
			if(Obj->GetType() == Defcon::EObjType::MINE && ENEMIES_MINESDONTHURT)
			{
				// For non-players, mines don't hurt.
				Obj2 = Obj2->GetNext();
				continue;
			}
			

			GetMainAreaMapper().To(Obj2->Position, ScreenPos);

			if(!(ScreenPos.x > -20 && ScreenPos.x < DisplayWidth + 20))
			{
				Obj2 = Obj2->GetNext();
				continue;
			}
				
			Bbox.Set(Obj2->Position);
			Bbox.Inflate(Obj2->BboxRadius);

			if(Bbox.PtInside(InjurePt) || Obj->TestInjury(Bbox))
			{
				// Object has been hit! Ow!!!

				// todo: generalize code so that all objects have a RegisterImpact() method.
				// Can't use dynamic_cast, force all objects to be static castable to ILiveGameObject.

				auto LiveObject = static_cast<Defcon::ILiveGameObject*>(Obj2);

				if(LiveObject != nullptr)
				{
					const bool bKilled = LiveObject->RegisterImpact(Obj->GetCollisionForce());

					if(bKilled)
					{
						if(Obj->GetType() == Defcon::EObjType::LASERBEAM)
						{
							if(Obj2->GetType() == Defcon::EObjType::HUMAN)
							{
								GDefconGameInstance->GetStats().FriendlyFireIncidents++;
							}
							else
							{
								GDefconGameInstance->GetStats().HostilesDestroyedByLaser++;
							}
						}

						DestroyObject(Obj2);
					}
				}

				// Either way, the weapons fire object is toast.
				Obj->MarkAsDead();

				// Skip testing targets, and continue with next bullet.
				break;
			}
			
			Obj2 = Obj2->GetNext();
		}
	});
}



void UDefconPlayViewBase::OnPlayerShipDestroyed()
{
	// Player died; restart mission.

	if(!bArenaClosing)
	{
		bArenaClosing = true;

		FadeAge = DESTROYED_PLAYER_LIFETIME * 2;

		Daylon::IScheduledTask* Task = nullptr;

		if(GetHumans().Count() == 0)
		{
			Task = new Defcon::CEndMissionTask;
		}
		else
		{
			Task = new Defcon::CRestartMissionTask;
		}

		Task->Countdown = FadeAge;

		ScheduledTasks.Add(Task);
	}
}


void UDefconPlayViewBase::ShieldBonk(Defcon::IGameObject* pObj, float fForce)
{
	// Make a small visual splash to indicate that 
	// an object's shields have been struck by 
	// a certain force.

	// todo: use playership.AddExplosionDebris

	const auto cby = Defcon::EColor::Gray;

	for(int32 I = 0; I < 10; I++)
	{
		auto Flak = new Defcon::CFlak;

		Flak->ColorbaseYoung = cby;
		Flak->LargestSize = 4;
		Flak->bFade = true;//bDieOff;

		Flak->Position    = pObj->Position;
		Flak->Orientation = pObj->Orientation;

		CFPoint Direction;
		Direction.SetRandomVector();

		// Debris has at least the object's momentum.
		Flak->Orientation.Fwd = pObj->Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		Flak->Orientation.Fwd *= FRANDRANGE(20, 32);
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRANDRANGE(110, 140);

		Flak->Orientation.Fwd.MulAdd(Direction, speed);

		Debris.Add(Flak);
	}

	m_fRadarFritzed = FMath::Max(1.5f, fForce * 10);//MAX_RADARFRITZ;

	GAudio->OutputSound(Defcon::EAudioTrack::Shieldbonk);
}


void UDefconPlayViewBase::ProcessWeaponsHits()
{
	CheckIfObjectsGotHit(Objects);
	CheckIfObjectsGotHit(Enemies);

	auto Mission = Defcon::GGameMatch->GetMission();

	if(Mission != nullptr && Mission->HumansInvolved())
	{
		CheckIfObjectsGotHit(GetHumans());
	}
}


void UDefconPlayViewBase::Hyperspace()
{
	// Make the player reappear elsewhere.

	if(!GetPlayerShip().IsAlive())
	{
		return;
	}

	// Randomize player ship position.
	GetPlayerShip().Position.Set(FRAND * ArenaWidth, FRANDRANGE(0.1f, 0.9f) * ArenaSize.Y);

	// todo: the original arcade game had a 25% chance of blowing up, 
	// we might want to do something similar. But if we do that, we should
	// also have a brief materialization effect.
	// e.g. if(FRAND < 0.25f) { DestroyPlayerShip ... }
}


void UDefconPlayViewBase::DetonateSmartbomb()
{
	if(GetPlayerShip().IsAlive())
	{
		GDefconGameInstance->GetStats().SmartbombsDetonated++;

		GAudio->OutputSound(Defcon::EAudioTrack::Smartbomb);

		auto SmartBombShockwave = new Defcon::CSmartbombShockwave;

		SmartBombShockwave->Position = GetPlayerShip().Position;

		SmartBombShockwave->InitSmartbombShockwave(CFPoint((float)MainAreaSize.X, MainAreaSize.Y), &GetMainAreaMapper(), &Enemies, &Debris);

		Blasts.Add(SmartBombShockwave);
	}
}


void UDefconPlayViewBase::SpecializeMaterialization(Defcon::FMaterializationParams& Params, Defcon::EObjType ObjectType)
{
	// We use a switch block because the enemy object doesn't exist (yet), so we can't 
	// call a virtual method on it. An example of where static virtual methods would be handy,
	// but since C++ can't do that, we should (todo) move this to a data table, either a 
	// UE asset or a TMap<EObjType, FMaterializationParams>. Either one would be slower, 
	// but otoh we don't materialize enemies often enough to matter.

	switch(ObjectType)
	{
		case Defcon::EObjType::SWARMER:
			Params.NumParticles       = 50;
			Params.StartingRadiusMin *= 0.5f;
			Params.StartingRadiusMax *= 0.5f;
			Params.ParticleSizeMin    = 2.0f;
			Params.ParticleSizeMax    = 5.0f;
			Params.AspectRatio        = 1.0f;
			Params.Colors             = { C_MAGENTA, C_RED, C_ORANGE };
			break;

		case Defcon::EObjType::GUPPY:
			Params.NumParticles       = 75;
			Params.StartingRadiusMin *= 0.75f;
			Params.StartingRadiusMax *= 0.75f;
			Params.ParticleSizeMin    = 2.5f;
			Params.ParticleSizeMax    = 5.5f;
			Params.AspectRatio        = 0.75f;
			Params.Colors             = { C_MAGENTA, C_MAGENTA, C_MAGENTA };
			break;

		case Defcon::EObjType::POD:
			Params.Colors             = { C_RED, C_MAGENTA, C_BLUE };
			break;

		case Defcon::EObjType::BOMBER:
			Params.Colors             = { C_BLUE, C_MAGENTA };
			break;

		case Defcon::EObjType::REFORMER:
			Params.Colors             = { C_RED, C_DARKRED, C_DARKER, C_DARKYELLOW };
			break;

		case Defcon::EObjType::FIREBOMBER_TRUE:
		case Defcon::EObjType::BOUNCER_TRUE:
			Params.NumParticles       = 150;
			Params.Colors             = { C_YELLOW };
			break;

		case Defcon::EObjType::FIREBOMBER_WEAK:
		case Defcon::EObjType::BOUNCER_WEAK:
			Params.NumParticles       = 150;
			Params.Colors             = { C_WHITE, C_LIGHTYELLOW, C_YELLOW };
			break;

		case Defcon::EObjType::HUNTER:
			Params.NumParticles       = 150;
			Params.Colors             = { C_LIGHTYELLOW, C_YELLOW, C_ORANGE };
			break;

		case Defcon::EObjType::BIGRED:
			Params.Colors             = { C_RED, C_ORANGE };
			break;

		case Defcon::EObjType::LANDER:
			Params.Colors             = { C_YELLOW, C_GREEN };
			break;

		case Defcon::EObjType::BAITER:
			Params.Colors             = { C_YELLOW, C_GREEN };
			Params.StartingRadiusMin *= 3.0f;
			Params.StartingRadiusMax *= 3.0f;
			Params.AspectRatio        = 4.0f;
			break;

		case Defcon::EObjType::MUNCHIE:
			Params.NumParticles       = 80;
			Params.StartingRadiusMin *= 0.75f;
			Params.StartingRadiusMax *= 0.75f;
			Params.ParticleSizeMin    = 2.5f;
			Params.ParticleSizeMax    = 5.5f;
			Params.Colors             = { C_GREEN, C_LIGHTGREEN };
			break;
	}
}


void UDefconPlayViewBase::SpawnGameObject
(
	Defcon::EObjType               ObjType, 
	Defcon::EObjType               CreatorType, 
	const CFPoint&                 Where, 
	float                          Countdown, 
	Defcon::EObjectCreationFlags   Flags, 
	const Defcon::FMetadata*       Options
)
{
	check(Where.IsValid());

	const auto MaterializationLifetime = ENEMY_BIRTHDURATION;

	if(Daylon::HasFlag(Flags, Defcon::EObjectCreationFlags::Materializes))
	{
		Defcon::FMaterializationParams Params;

		Params.Lifetime          = MaterializationLifetime;
		Params.P                 = Where;
		Params.NumParticles      = 100;
		Params.OsMin             = 0.0f;
		Params.OsMax             = 1.0f;
		Params.ParticleSizeMin   = 3.0f;
		Params.ParticleSizeMax   = 6.0f;
		Params.StartingRadiusMin = ENEMY_BIRTHDEBRISDIST * 0.1f;
		Params.StartingRadiusMax = ENEMY_BIRTHDEBRISDIST;
		Params.AspectRatio       = 1.5f;
		Params.Colors            = { C_WHITE, C_RED, C_YELLOW, C_ORANGE, C_LIGHTYELLOW };
		Params.TargetBoxRadius.Set(0.0f, 0.0f);

		SpecializeMaterialization(Params, ObjType);

		/*if(Options != nullptr)
		{
			Params.Extra = *Options;
		}*/

		auto MaterializationTask = new Defcon::CCreateMaterializationTask;
		MaterializationTask->InitMaterializationTask(Params);
		MaterializationTask->Countdown = Countdown;

		Countdown += MaterializationLifetime;

		Defcon::GGameMatch->GetMission()->AddTask(MaterializationTask);
	}

	Defcon::GGameMatch->GetMission()->AddGameObject(ObjType, CreatorType, Where, Countdown, Flags, Options);
}


Defcon::CEnemy* UDefconPlayViewBase::SpawnGameObjectNow(Defcon::EObjType ObjType, Defcon::EObjType CreatorType, const CFPoint& Where, Defcon::EObjectCreationFlags Flags)
{
	// Create an enemy immediately by executing its Do method and not putting the task into a task list.

	auto Task = new Defcon::CCreateGameObjectTask;

	Task->ObjType        = ObjType;
	Task->CreatorType    = CreatorType;
	Task->Where          = Where;
	Task->bMissionTarget = Daylon::HasFlag(Flags, Defcon::EObjectCreationFlags::IsMissionTarget);
	
	Task->Do();

	return (Defcon::CEnemy*)GetEnemies().GetFirst();
}


void UDefconPlayViewBase::OnSelectGameObjectToSpawn()
{
	// Debug tool, lets us spawn enemies on demand to test materialization, etc.

	SpawnedGameObjectIndex = (SpawnedGameObjectIndex + 1) % array_size(SpawnedGameObjectTypes);

	FString Str = FString::Printf(TEXT("Enemy type %s chosen"), *Defcon::GObjectTypeManager.GetName(SpawnedGameObjectTypes[SpawnedGameObjectIndex]));

	Defcon::GMessageMediator.TellUser(Str, 0.25f);
}


void UDefconPlayViewBase::OnSpawnGameObject()
{
	// Debug tool, lets us spawn enemies on demand to test materialization, etc.

	auto P = GetPlayerShip().Position;
	P.x = WrapX(P.x + 300 * SGN(GetPlayerShip().Orientation.Fwd.x));

	SpawnGameObject(SpawnedGameObjectTypes[SpawnedGameObjectIndex], Defcon::EObjType::UNKNOWN, P, 0.0f, 
		(Defcon::EObjectCreationFlags)((int32)Defcon::EObjectCreationFlags::Materializes | (int32)Defcon::EObjectCreationFlags::NotMissionTarget));
}


void UDefconPlayViewBase::OnHumansChanged()
{
	// Can also be called when a human dies.

	TArray<bool> AbductionStates;
	AbductionStates.Reserve(GetHumans().Count());

	GetHumans().ForEach([&AbductionStates](Defcon::IGameObject* Obj)
		{
			auto Human = static_cast<Defcon::CHuman*>(Obj);

			AbductionStates.Add(Human->IsBeingAbducted());
		});

	Defcon::GMessageMediator.Send(Defcon::EMessageEx::AbductionCountChanged, &AbductionStates);
}


#undef MAX_RADARFRITZ
#undef FADE_DURATION	
#undef FADE_DURATION_NORMAL
#undef PLAYERSHIP_VMARGIN


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
