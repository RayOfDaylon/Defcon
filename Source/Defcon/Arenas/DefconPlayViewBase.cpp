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
#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "DaylonUtils.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
//#include "UMG/Public/Components/CanvasPanelSlot.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


// -----------------------------------------------------------------------------------------

#define MAX_RADARFRITZ	             3.0f
#define FADE_DURATION			     1.5f
#define FADE_DURATION_NORMAL	     0.25f // Controls how long to stay in arena while we teleport to next mission
#define PLAYERSHIP_VMARGIN          20.0f  // Keep player ship at least this far from top and bottom main widget edges
#define DURATION_IMPORTANT_MESSAGE   4.0f


UDefconPlayViewBase* GArena = nullptr;



float GameTime() { check(GArena != nullptr); return UKismetSystemLibrary::GetGameTimeInSeconds(GArena); }



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

	if(bDoneActivating)
	{
		UpdateGameObjects(DeltaTime);
	}

	if(Fader->IsVisible())
	{
		const float T = 1.0f - FMath::Max(0.0f, FadeAge / FADE_DURATION_NORMAL);

		Fader->SetRenderOpacity(T);
	}
}


#if 0
int32 UDefconPlayViewBase::NativePaint
(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
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

	return LayerId;
}
#endif


const Defcon::CGameObjectCollection& UDefconPlayViewBase::GetConstHumans() const 
{ 
	return GetConstDefconGameInstance()->GetConstHumans();
}


Defcon::CGameObjectCollection& UDefconPlayViewBase::GetHumans()
{
	return GDefconGameInstance->GetHumans();
}


void UDefconPlayViewBase::DeleteAllObjects()
{
	Enemies.  DeleteAll(Defcon::kIncludingSprites);
	Blasts.   DeleteAll(Defcon::kIncludingSprites);
	Debris.   DeleteAll(Defcon::kIncludingSprites);
	Objects.DeleteAll(Defcon::kIncludingSprites);

	Events.   DeleteAll();
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

	GArena = this;

	bArenaDying = false;
	Daylon::Hide(Fader);

	ShipThrustSoundLoop = gpAudio->CreateLoopedSound(Defcon::EAudioTrack::Playership_thrust);
	WasShipUnderThrust = false;

	InitMapperAndTerrain();
	InitPlayerShip();

	PlayAreaMain->Init(&GetHumans(), &Objects, &Enemies, &Debris, &Blasts, ArenaSize);

	// Start the current mission.
	auto GI = GDefconGameInstance;

	if(!IsValid(GI))
	{
		return;
	}

	GI->InitMission();

	m_bHumansInMission = GI->GetMission()->HumansInvolved();



	PlayAreaRadar->Init(&GetPlayerShip(), MainAreaSize, (int32)ArenaWidth, &MainAreaMapper, &Objects, &Enemies);


	GetPlayerShip().BindToShieldValue([WeakThis = TWeakObjectPtr<UDefconPlayViewBase>(this)](const float& Value)
	{
		if(auto This = WeakThis.Get())
		{
			This->PlayAreaStats->UpdateShieldReadout(FMath::Clamp(Value, 0.0f, 1.0f));
		}
	});

	GDefconGameInstance->BindToSmartbombCount([WeakThis = TWeakObjectPtr<UDefconPlayViewBase>(this)](const int32& Value)
	{
		if(auto This = WeakThis.Get())
		{
			This->PlayAreaStats->UpdateSmartbombReadout(Value);
		}
	});


	PlayAreaMain  -> SetSafeToStart();
	PlayAreaRadar -> OnFinishActivating();

	GetPlayerShip().EnableInput();

	gpAudio->OutputSound(Defcon::EAudioTrack::Wave_start);
}


void UDefconPlayViewBase::OnDeactivate() 
{
	// We're being transitioned away from.
	// Empty all our object collections.

	LOG_UWIDGET_FUNCTION
	Super::OnDeactivate();

	PlayAreaMain->OnDeactivate();
	PlayAreaRadar->OnDeactivate();

	// Because the object collections are owned by us and not by the current mission,
	// object deletions can happen after the current mission has been recreated 
	// due to the player ship getting destroyed, in which case it will attempt 
	// to remove hostiles in a new (and empty mission), borking the hostile count.
	// So for now, mark all enemies as non-mission-critical.

	Enemies.ForEach([](Defcon::IGameObject* Obj){ Obj->SetAsMissionTarget(false); });

	DeleteAllObjects();

	// Uninstall sprites for any surviving humans.
	//if(m_bHumansInMission)
	//{
		//GetHumans().ForEach([](Defcon::IGameObject* Ptr) { Ptr->UninstallSprite(); });
	//}

	//GetPlayerShip().UninstallSprite();

	AllStopPlayerShip();
	GetPlayerShip().EnableInput(false);
}


bool UDefconPlayViewBase::IsOkayToFinishActivating() const
{
	auto const S = Daylon::GetWidgetSize(PlayAreaMain);

	return (S.X > 0.0f && S.Y > 0.0f);
}


void UDefconPlayViewBase::InitMapperAndTerrain()
{
	MainAreaSize = Daylon::GetWidgetSize(PlayAreaMain);

	check(MainAreaSize.X > 0.0f && MainAreaSize.Y > 0.0f);

	ArenaSize = MainAreaSize * FVector2D(PLANET_TO_SCREEN_RATIO, 1.0);

	const auto MainS = Daylon::GetWidgetSize(PlayAreaMain);

	ArenaWidth = ArenaSize.X;
	MainAreaMapper.Init((int32)MainS.X, (int32)MainS.Y, (int32)ArenaWidth);
	PlayAreaMain->CoordMapperPtr = &MainAreaMapper;

	SAFE_DELETE(Terrain);
	
	// todo: if the current mission has no terrain, set it to nothing.
	// todo: we need to delete Terrain and set it to nullptr when needed.
	PlayAreaMain->TerrainPtr = nullptr;
	PlayAreaRadar->SetTerrain(nullptr);
}


void UDefconPlayViewBase::InitPlayerShip()
{
	// Set up the player ship for the start of a mission.

	LOG_UWIDGET_FUNCTION
	auto& PlayerShip = GetPlayerShip();

	PlayerShip.MapperPtr = &MainAreaMapper;
	PlayerShip.InitPlayer(ArenaWidth);

	// Position the ship at x=0, and halfway up.
	PlayerShip.Position.Set(0.0f, ArenaSize.Y / 2);
	PlayerShip.FaceRight();
	
	PlayAreaMain->PlayerShipPtr = &PlayerShip;

	// Sprite installation for player ship happens in DefconPlayMainWidgetBase
	PlayerShip.SetIsAlive(true);

	PlayerShip.Orientation.Fwd.x = 1.0f;

	AllStopPlayerShip();
}


void UDefconPlayViewBase::AddMessage(const FString& Str, float Duration)
{
	PlayAreaMain->AddMessage(Str, Duration);
}


void UDefconPlayViewBase::TransportPlayerShip()
{
	// The player ship entered the stargate but the mission isn't over.
	// If any humans are being abducted, place the ship at the highest one
	// and maintain the ship's direction.
	//   If no abductions, take the player to where the action is.

	CFPoint pt(0.0f, 0.0f);

	if(m_bHumansInMission)
	{
		GetConstHumans().ForEach([&](Defcon::IGameObject* Object)
		{
			auto Human = static_cast<Defcon::CHuman*>(Object);

			if(Human->IsBeingCarried() && Human->GetCarrier()->GetType() != Defcon::EObjType::PLAYER)
			{
				// Track highest human but not one that is too close to the top of the arena.
				if(Human->Position.y > pt.y && Human->Position.y < ArenaSize.Y - 150)
				{
					pt = Human->Position;
				}
			}
		});
	}

	auto& PlayerShip = GetPlayerShip();

	const auto Mission = static_cast<Defcon::CMilitaryMission*>(GDefconGameInstance->GetMission());

	if(pt.y != 0.0f)
	{
		// A human is being abducted.

		// Move player to where human is, plus some height so the lander can be targeted.

		PlayerShip.Position = pt;
		PlayerShip.Position.y += 100;

		// Move player horizontally by half the screen width, but facing the human.
		// If the player is still inside the stargate, adjust the distance until he's clear.
		float DistanceFactor = 0.4f;

		do
		{
			DistanceFactor += 0.1f;

			PlayerShip.Position.x = WrapX(pt.x - (PlayerShip.Orientation.Fwd.x * GetDisplayWidth() * DistanceFactor));

		} while(Mission->PlayerInStargate());
	}
	else
	{
		// No humans are being abducted. 
		// For now, teleport the player randomly.

		do
		{
			PlayerShip.Position.Set(WrapX(FRAND * ArenaWidth), (FRAND * 0.8f + 0.1f) * ArenaSize.Y);
		} while(Mission->PlayerInStargate());
	}

	// The SettlePlayer routine assumes the player ship is visible, and when it's not, 
	// it just slowly pans the arena until it is. We have to pan the arena immediately 
	// so that SettlePlayer doesn't have to do anything.

	// Get the player ship's screen location. If it's not at the margin (and it almost certainly won't be)
	// then we need to pan by the difference.

	CFPoint ScreenPt;
	MainAreaMapper.To(PlayerShip.Position, ScreenPt);

	const float kMargin = PLAYER_POSMARGIN;

	if(PlayerShip.Orientation.Fwd.x > 0)
	{
		// Player facing right.
		const float dx = ScreenPt.x - kMargin;
		MainAreaMapper.SlideBy(dx);
	}
	else
	{
		// Player facing left.
		const float dx = GetDisplayWidth() - kMargin - ScreenPt.x;
		MainAreaMapper.SlideBy(-dx);
	}
}


void UDefconPlayViewBase::SettlePlayer(float DeltaTime)
{
	// This routine is like a deamon which works to 
	// keep the player ship within some fixed margin 
	// of the screen edge being pointed away from.
	// That way, if the user reverses direction and 
	// does nothing else, the screen will quietly pan 
	// to put most of the arena in front of him.

	// rcg, jun 24/04: a ship with a larger maxthrust 
	// is able to outrun the settler. Increasing 
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

	if(PlayerShip.Orientation.Fwd.x > 0)
	{
		// Player facing right.
		const float dx = ScreenPt.x - kMargin;
		const float f  = FMath::Min(kPanAvail, dx);
		MainAreaMapper.SlideBy(f);
	}
	else
	{
		// Player facing left.
		const float dx = GetDisplayWidth() - kMargin - ScreenPt.x;
		const float f  = FMath::Min(kPanAvail, dx);
		MainAreaMapper.SlideBy(-f);
	}
}


void UDefconPlayViewBase::UpdatePlayerShipInputs()
{
	// No explicit reverse, map events directly to thrusts.

	auto& PlayerShip = GetPlayerShip();

	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlFwd,  MoveShipRightState.bActive, MoveShipRightState.TimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlBack, MoveShipLeftState.bActive,  MoveShipLeftState.TimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlUp,   MoveShipUpState.bActive,    MoveShipUpState.TimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlDown, MoveShipDownState.bActive,  MoveShipDownState.TimeDown);
}


void UDefconPlayViewBase::AllStopPlayerShip()
{
	MoveShipRightState.bActive = 
	MoveShipLeftState.bActive =
	MoveShipUpState.bActive =
	MoveShipDownState.bActive = false;

	auto& PlayerShip = GetPlayerShip();

	PlayerShip.ZeroInput();
	PlayerShip.ZeroMotion();
	PlayerShip.ZeroThrust();
	PlayerShip.ZeroVelocity();
}


void UDefconPlayViewBase::IncreaseScore(int32 Points, bool bVis, const CFPoint* pPos)
{
	GDefconGameInstance->AdvanceScore((int32)Points);
}


float UDefconPlayViewBase::GetTerrainElev(float X) const
{
	check(X >= 0 && X <= GetWidth());

	return Terrain->GetElev(X / GetWidth());
}


void UDefconPlayViewBase::ConcludeMission()
{
	// Wave can end. todo: handle via gameinstance?

	if(!bArenaDying)
	{
		bArenaDying = true;
		auto pEvt = new Defcon::CEndMissionEvent;
		pEvt->Init();
		//pEvt->m_what = CEvent::Type::endmission;
		pEvt->When = GameTime() + FADE_DURATION_NORMAL;
		Events.Add(pEvt);

		FadeAge = FADE_DURATION_NORMAL;

		Daylon::Show(Fader);
	}
}

constexpr float DESTROYED_PLAYER_FLASH_DURATION        = 1.5f;
constexpr float DESTROYED_PLAYER_EXPLOSION_DURATION    = 2.0f;
constexpr float DESTROYED_PLAYER_BLANK_DURATION        = 2.0f / 60.0f; // time to flash the fullscreen 50% opaque white
constexpr float DESTROYED_PLAYER_LIFETIME              = DESTROYED_PLAYER_FLASH_DURATION + DESTROYED_PLAYER_EXPLOSION_DURATION;
constexpr float DESTROYED_PLAYER_PARTICLE_SPEED        = 30.0f;
constexpr float DESTROYED_PLAYER_PARTICLE_MIN_LIFETIME = 1.0f; 
constexpr float DESTROYED_PLAYER_PARTICLE_MAX_LIFETIME = DESTROYED_PLAYER_EXPLOSION_DURATION;


struct Particle
{
	FLinearColor Color;
	CFPoint      P;
	CFPoint      Inertia;
	float        S;
	float        Age = 0.0f;
	float        Lifetime = 0.0f;

	void Update(float DeltaTime)
	{
		Age += DeltaTime;
		P += Inertia * DeltaTime;
	}

	void Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)
	{
		CFPoint pt;
		CoordMapper.To(P, pt);
		const float HalfS = S / 2;

		Color.A = 1.0f - CLAMP(NORM_(Age, 0.0f, Lifetime), 0.0f, 1.0f);

		Painter.FillRect(pt.x - HalfS, pt.y - HalfS, pt.x + HalfS, pt.y + HalfS, Color);
	}
};


struct ParticleGroup
{
	Particle Particles[8][2];
	CFPoint  P;


	ParticleGroup()
	{
		for(int32 y = 0; y < 2; y++)
		{
			for(int32 x = 0; x < 8; x++)
			{
				Particles[x][y].Color = BRAND ? C_WHITE : C_YELLOW;
				Particles[x][y].S = Daylon::FRandRange(3.0f, 9.0f);
			}
		}
	}


	void Init(const CFPoint& ShipP, const CFPoint& ShipHalfS, float ParticleSpeed, float MinParticleLifetime, float MaxParticleLifetime)
	{
		const CFPoint OriginShift((float)Daylon::FRandRange(-ShipHalfS.x * 1, ShipHalfS.x * 1), (float)Daylon::FRandRange(-ShipHalfS.y * 0.75, ShipHalfS.y * 0.75));
		const auto Origin = ShipP + OriginShift;

		for(int32 y = 0; y < 2; y++)
		{
			const float ty = y;

			for(int32 x = 0; x < 8; x++)
			{
				const float tx = x / 7.0f;

				Particles[x][y].Lifetime = Daylon::FRandRange(MinParticleLifetime, MaxParticleLifetime);
				Particles[x][y].P        = Origin + CFPoint(LERP(-ShipHalfS.x, ShipHalfS.x, tx), LERP(-ShipHalfS.y, ShipHalfS.y, ty));
				Particles[x][y].Inertia  = (Particles[x][y].P - ShipP) * ParticleSpeed;
			}
		}
	}


	void Update(float DeltaTime)
	{
		for(int32 y = 0; y < 2; y++)
		{
			for(int32 x = 0; x < 8; x++)
			{
				Particles[x][y].Update(DeltaTime);
			}
		}
	}


	void Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)
	{
		for(int32 y = 0; y < 2; y++)
		{
			for(int32 x = 0; x < 8; x++)
			{
				Particles[x][y].Draw(Painter, CoordMapper);
			}
		}
	}
};


class CDestroyedPlayerShip : public Defcon::ILiveGameObject
{
	private:

		ParticleGroup     ParticleGroups[100];

		// We can update and draw a subset of the particle groups every frame,
		// to mimic the arcade game better.
		int32             NumParticleGroupsToUpdate = 1;
		int32             NumParticleGroupsToDraw = array_size(ParticleGroups);

	public:

		CDestroyedPlayerShip()
		{
			Type          = Defcon::EObjType::DESTROYED_PLAYER;
			CreatorType   = Defcon::EObjType::PLAYER;
			Lifespan     = DESTROYED_PLAYER_LIFETIME;
			bMortal       = true;
			bCanBeInjured = false;
			Age          = 0.0f;

			CreateSprite(Defcon::EObjType::DESTROYED_PLAYER);
		}


		void InitDestroyedPlayer(const CFPoint& ShipP, const CFPoint& ShipS, float ParticleSpeed, float MinParticleLifetime, float MaxParticleLifetime)
		{
			for(auto& ParticleGroup : ParticleGroups)
			{
				ParticleGroup.Init(ShipP, ShipS, ParticleSpeed, MinParticleLifetime, MaxParticleLifetime);
			}
		}


		virtual void Move(float DeltaTime) override
		{
			Age += DeltaTime;

			if(Age > DESTROYED_PLAYER_FLASH_DURATION)
			{
				// After the initial red-white flicker, stop showing the ship
				//Sprite->Hide();
				// Uninstall and reset the sprite; this will cause paint events to call Draw method.
				if(Sprite)
				{
					UninstallSprite();
					Sprite.Reset();

					gpAudio->OutputSound(Defcon::EAudioTrack::Ship_exploding2b);
				}

				// Move the particle groups.

				for(int32 Index = 0; Index < NumParticleGroupsToUpdate; Index++)
				{
					auto& ParticleGroup = ParticleGroups[Index];
			
					ParticleGroup.Update(DeltaTime);
				}

				NumParticleGroupsToUpdate = FMath::Min(NumParticleGroupsToUpdate + 2, (int32)array_size(ParticleGroups));

			}
		}


		virtual void Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)  override
		{
			// Our sprite will autodraw for the first 0.5 seconds, after that
			// we need to draw explosion debris.

			if(Age <= DESTROYED_PLAYER_FLASH_DURATION)
			{
				return;
			}

			// Flash a few frames of increasingly transparent white.
			if(Age < DESTROYED_PLAYER_FLASH_DURATION + DESTROYED_PLAYER_BLANK_DURATION)
			{
				// If we miss out on the fullscreen blank because of timing skips,
				// we can always use a frame count flag to force it.
				// It looks like the Xbox screen capture recording app can't see it, even though UE has a 60 fps frame rate.
				// Could be that the sudden change in screen content is too short to let the recorder save an I-frame.

				FLinearColor Color = C_WHITE;
				Color.A = NORM_(Age, DESTROYED_PLAYER_FLASH_DURATION, DESTROYED_PLAYER_FLASH_DURATION + DESTROYED_PLAYER_BLANK_DURATION);
				Color.A = CLAMP(Color.A, 0.0f, 1.0f);
				
				Painter.FillRect(0.0f, 0.0f, Painter.GetWidth(), Painter.GetHeight(), Color);

				return;
			}

			// Draw something really funky here.
			// It looks like 8-12 particles per clump, and 
			// 12 x 5 = 60 clumps. The ones on the middle row 
			// spread out thinly since they don't have a real vertical aspect.
			// or 8 x 2 initially...
			// It looks like there are several sets of 8 x 2 patterns, 
			// composed of 2 x 2 pixel dots, some all lit, others having 
			// only 3 lit, chosen randomly, or the individual pixel colors
			// include black which is chosen sparingly.
			// The multiple clumpsets make sense because it looks like each 
			// set expands by simply inserting gaps between dots, 
			// using two pixels across and down each frame.
			// By superimposing other sets slightly offset, a rich 
			// overall explosion can be produced.
			// So the very first clump, all the dots are touching.
			// On the next frame, the clump is twice as wide and tall
			// with a 2 px gap between each dot in both x and y.
			// The next frame, each dot has a 4 px gap, and so on.
			// The top of the clump shifts down (or the bottom of it shifts up)
			// depending on how the clump was positioned relative to the ship.

			// So 16 dots by e.g. 5 clumpsets = 80 dots, although I think 
			// there will be many more.

			// As we place each set, we need to compute a vector from the 
			// ship center to get the direction of each dot.

			for(int32 Index = 0; Index < NumParticleGroupsToDraw; Index++)
			{
				auto& ParticleGroup = ParticleGroups[Index];
			
				ParticleGroup.Draw(Painter, CoordMapper);
			}

			NumParticleGroupsToDraw = FMath::Min(NumParticleGroupsToDraw + 2, (int32)array_size(ParticleGroups));
		}

};


void UDefconPlayViewBase::DestroyPlayerShip()
{
	auto& PlayerShip = GetPlayerShip();

	AllStopPlayerShip();
	PlayerShip.EnableInput(false);
	PlayerShip.Lifespan = 0.0f;
	PlayerShip.SetIsAlive(false);

	// Start the player ship destruction sequence.
	// We do this with a special game object.

	auto pShip = new CDestroyedPlayerShip();

	pShip->InitDestroyedPlayer(PlayerShip.Position, PlayerShip.BboxRadius, DESTROYED_PLAYER_PARTICLE_SPEED, DESTROYED_PLAYER_PARTICLE_MIN_LIFETIME, DESTROYED_PLAYER_PARTICLE_MAX_LIFETIME);

	pShip->Position        = PlayerShip.Position;
	pShip->Orientation.Fwd = PlayerShip.Orientation.Fwd;

	pShip->Sprite->FlipHorizontal = (pShip->Orientation.Fwd.x < 0);

	pShip->InstallSprite();

	Objects.Add(pShip);

	gpAudio->OutputSound(Defcon::EAudioTrack::Player_dying);

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

				const bool bPlayerKilled = !GDefconGameInstance->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce());

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
					OnPlayerShipDestroyed();
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

				const bool bPlayerKilled = !GDefconGameInstance->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce());

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
					if(!bPlayerKilled)
					{
						ExplodeObject(pObj);
					}
					else
					{
						IncreaseScore(pObj->GetPointValue(), true, &pObj->Position);
						pObj->bMortal = true;
						pObj->Lifespan = 0.0f;
						pObj->OnAboutToDie();
					}
				}

				if(bPlayerKilled)
				{
					OnPlayerShipDestroyed();
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


void UDefconPlayViewBase::CreateTerrain()
{
	SAFE_DELETE(Terrain);
	Terrain = new Defcon::CTerrain;
	Terrain->InitTerrain(GetWidth(), GetHeight());

	PlayAreaMain->TerrainPtr = Terrain;
	PlayAreaRadar->SetTerrain(Terrain);
}


Defcon::CPlayer& UDefconPlayViewBase::GetPlayerShip() 
{
	return GDefconGameInstance->GetPlayerShip();
}


void UDefconPlayViewBase::UpdateGameObjects(float DeltaTime)
{
	if(!bArenaDying)
	{
		if(!GDefconGameInstance->Update(DeltaTime))
		{
			ConcludeMission();
		}
	}

	Events.Process(DeltaTime);


	if(FadeAge > 0.0f)
	{
		FadeAge -= DeltaTime;
	}

	if(m_bRunSlow)
	{
		DeltaTime *= 0.25f;
	}
	else if(ARENA_BOGDOWN)
	{
		if(Enemies.Count() + Objects.Count() + Debris.Count() > 250)
		{
			if(Enemies.Count() + Objects.Count() < 500)
			{
				DeltaTime *= 0.8f;
			}
			else if(Enemies.Count() + Objects.Count() < 750)
			{
				DeltaTime *= 0.65f;
			}
			else //if(Objects.Count() < 750)
			{
				DeltaTime *= 0.5f;
			}
		}
	}


	ProcessWeaponsHits();

	CheckPlayerCollided();
	CheckIfPlayerHit(Objects);

	// Move and draw everyone.
	//m_virtualScreen.Clear(gGameColors.GetColor(0, (float)m_nFlashScreen/SMARTBOMB_MAX_FLASHSCREEN));

	if(m_nFlashScreen > 0)
	{
		m_nFlashScreen--;
	}

	const auto Mission = GDefconGameInstance->GetMission();
	
	const bool HumansInvolved = Mission != nullptr ? Mission->HumansInvolved() : false;

	// ------------------------------------------------------
	// Process objects (move, draw, delete).

	static int32 n = 0;
	const bool bDrawRadar = (n++ % 5 == 0);

	// Note: update only model data, not visual.
	Defcon::GameObjectProcessingParams gop;

	gop.UninstallSpriteIfObjectDeleted = true;
	gop.DeltaTime	= DeltaTime;
	gop.MapperPtr   = &MainAreaMapper;

	Blasts.Process(gop);
	Debris.Process(gop);

	if(HumansInvolved)
	{
		const auto NumHumansBefore = GetHumans().Count();
		GetHumans().Process(gop);
		const auto NumHumansNow = GetHumans().Count();

		if(NumHumansNow < NumHumansBefore)
		{
			FString Str;

			if(NumHumansNow > 0)
			{
				Str = FString::Printf(TEXT("HUMANS REMAINING: %d"), NumHumansNow);
			}
			else
			{
				Str = TEXT("ALL HUMANS ABDUCTED OR KILLED");
			}
			AddMessage(Str, 3.0f);
		}
	}

	gop.OnDeath = [this](Defcon::IGameObject* ObjPtr)
	{
		const auto Mission = GDefconGameInstance->GetMission();
		
		if(!Mission->IsMilitary())
		{
			return;
		}

		const auto MilitaryMission = static_cast<Defcon::CMilitaryMission*>(Mission);

		if(ObjPtr->GetType() == Defcon::EObjType::LANDER)
		{
			if(MilitaryMission->LandersRemaining() == 1)
			{
				AddMessage(TEXT("ALL LANDERS DESTROYED"), DURATION_IMPORTANT_MESSAGE);
			}
		}

		if(ObjPtr->IsMissionTarget() && MilitaryMission->HostilesRemaining() == 1)
		{
			AddMessage(TEXT("ALL MISSION TARGETS DESTROYED -- PROCEED TO STARGATE"), DURATION_IMPORTANT_MESSAGE);
		}
	};


	Objects.Process(gop);
	Enemies.Process(gop);

	// ------------------------------------------------------

	const int32 w  = MainAreaSize.X;
	const int32 h  = MainAreaSize.Y;
	//const int32 hr = m_virtualRadarScreen.GetHeight();
	const int32 wp = (int32)ArenaWidth;// GetWidth();

	const float now = GameTime();

	auto& PlayerShip = GetPlayerShip();


	if(PlayerShip.IsAlive())
	{
		UpdatePlayerShipInputs();

		PlayerShip.Move(DeltaTime);

		// Constrain player vertically.
		PlayerShip.Position.y = CLAMP(PlayerShip.Position.y, PLAYERSHIP_VMARGIN, ArenaSize.Y - PLAYERSHIP_VMARGIN);

		SettlePlayer(DeltaTime);
		DoThrustSound(DeltaTime);

		// Handle wraparound onto planet.
		if(PlayerShip.Position.x < 0)
		{
			 PlayerShip.Position.x += ArenaWidth;
		}
		else if(PlayerShip.Position.x >= ArenaWidth)
		{
			PlayerShip.Position.x -= ArenaWidth;
		}


		//if(PlayerShip.IsSolid())
		{
			// Things to do if player is solid.
			//static int mod = 0;

			// See if any carried humans can be debarked.
			if(/*mod++ % 5 == 0 && */PlayerShip.Position.y < 5.0f + GetTerrainElev(PlayerShip.Position.x))
			{
				if(PlayerShip.DebarkOnePassenger(GetHumans()))
				{
					IncreaseScore((int32)HUMAN_VALUE_DEBARKED, true, &PlayerShip.Position);
				}
			}
			else
			{
				// See if any falling humans can be carried.
				if(HumansInvolved)
				{
					// todo: not sure why we're hit testing projected bboxes of player and humans, world space ought to be fine.
					CFPoint playerScreenPos;
					MainAreaMapper.To(PlayerShip.Position, playerScreenPos);

					CFRect rPlayer(playerScreenPos);
					rPlayer.Inflate(PlayerShip.GetPickupRadiusBox());

					//Defcon::IGameObject* pObj = GetHumans().GetFirst();
					GetHumans().ForEachUntil([&](Defcon::IGameObject* pObj)
					//while(pObj != nullptr)
					{
						CFPoint humanScreenPos;
						MainAreaMapper.To(pObj->Position, humanScreenPos);

						if(rPlayer.Intersect(CFRect(humanScreenPos)))
						{
							Defcon::CHuman& Human = *((Defcon::CHuman*)pObj);

							if(Human.IsFalling())
							{
								if(PlayerShip.EmbarkPassenger(pObj, GetHumans()))
								{
									IncreaseScore((int32)HUMAN_VALUE_EMBARKED, true, &PlayerShip.Position);
								}
								return false;
							}
						}
						return true;
						//pObj = pObj->GetNext();
					});
				}
			}
		} // if player is solid.
	}
	else
	{
		// The player doesn't exist.
		// If he has ships left, then
		// When the player rebirth clock runs down,
		// rebirth the player. Otherwise, end the game.

		// todo: when the player ship finishes materializing, make it visible.
#if 0
		if(gpGame->PlayerLivesLeft())
		{
			m_fPlayerRebirthClock -= DeltaTime;
			m_fPlayerRebirthClock = FMath::Max(0, m_fPlayerRebirthClock);
			if(m_fPlayerRebirthClock == 0)
			{
				m_pPlayer = new CPlayer;
				m_coordMapper.From(
					CFPoint(m_virtualScreen.GetWidth()/2, 
					m_virtualScreen.GetHeight()/2), 
					m_pPlayer->Position);
				m_pPlayer->Init((float)wp);
				m_pPlayer->MapperPtr = &m_coordMapper;
				Objects.Add(m_pPlayer);
				
				m_fPlayerRebirthClock = PLAYER_REBIRTH_DELAY;
			}
		}
		else
#endif
		{
			// Player died; restart mission.
			if(!bArenaDying)
			{
				bArenaDying = true;
				auto pEvt = new Defcon::CRestartMissionEvent;
				pEvt->Init();

				pEvt->When = now + FADE_DURATION_NORMAL;
				Events.Add(pEvt);

				FadeAge = FADE_DURATION_NORMAL;
			}
		}
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
	TransitionToArena(EDefconArena::MainMenu);
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
			//SetCurrentCel(PlayerShipThrustingAtlasCel);
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
	return (MoveShipLeftState.bActive || MoveShipRightState.bActive);
}


void UDefconPlayViewBase::OnPawnNavEvent(EDefconPawnNavigationEvent Event, bool Active)
{
	// Move the player ship. todo: could be done in gameinstance.


	auto& PlayerShip = GetPlayerShip();

	if(!(PlayerShip.IsAlive() && PlayerShip.IsInputEnabled()))
	{
		return;
	}

	FDefconInputState* InputStatePtr = nullptr;

	switch(Event)
	{
		case EDefconPawnNavigationEvent::Up:    InputStatePtr = &MoveShipUpState;    break;
		case EDefconPawnNavigationEvent::Down:  InputStatePtr = &MoveShipDownState;  break;
		case EDefconPawnNavigationEvent::Left:  InputStatePtr = &MoveShipLeftState;  PlayerShip.FaceLeft();  break;
		case EDefconPawnNavigationEvent::Right: InputStatePtr = &MoveShipRightState; PlayerShip.FaceRight(); break;
		case EDefconPawnNavigationEvent::Hyperspace: Hyperspace(); return;
	
		default:
			return;
	}

	const bool EventBecameActive = !InputStatePtr->bActive && Active;
	InputStatePtr->bActive = Active;
	
	if(EventBecameActive) 
	{
		InputStatePtr->TimeDown = GameTime(); 
	}
}


void UDefconPlayViewBase::OnNavEvent(ENavigationKey Key)
{
	// debugging only
	const float Direction = (Key == ENavigationKey::Left ? -1.0f : 1.0f);

	CFPoint pt = MainAreaMapper.GetOffset();
	pt.x += Direction * 100.0f;
	pt.x = FMath::Wrap(pt.x, 0.0f, MainAreaMapper.PlanetCircumference);
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
				GDefconGameInstance->GetStats().ShotsFired++;

				if(true/*BRAND*/)
					gpAudio->OutputSound(Defcon::EAudioTrack::Laserfire);
				else
					gpAudio->OutputSound(Defcon::EAudioTrack::Laserfire_alt);
			}

		break;


		case EDefconPawnWeaponEvent::FireSmartbomb: 

			if(GetPlayerShip().IsAlive() && GDefconGameInstance->AcquireSmartBomb())
			{
				FireSmartbomb();
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


typedef enum { fa_wild, fa_at, fa_lead } FiringAccuracy;


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


void UDefconPlayViewBase::LayMine(Defcon::IGameObject& Obj, const CFPoint& from, int, int)
{
	auto Mine = new Defcon::CMine;

	Mine->InstallSprite();
	Mine->Position = Obj.Position - Obj.Orientation.Fwd * (2 * FMath::Max(Obj.BboxRadius.x, Obj.BboxRadius.y));

	Objects.Add(Mine);
}


Defcon::IBullet* UDefconPlayViewBase::FireBullet(Defcon::IGameObject& obj, const CFPoint& from, int soundid, int)
{
	//check(m_pPlayer != nullptr);
	Defcon::IBullet* pBullet;

	if(obj.GetType() == Defcon::EObjType::GUPPY)
	{
		pBullet = new Defcon::CThinBullet;
	}
	else
	{
		pBullet = new Defcon::CBullet;
	}

		
	//auto pBullet = new Defcon::CBullet;
	pBullet->SetCreatorType(obj.GetType());
	pBullet->InstallSprite();

	// Scores at which to switch firing style.
	const int32 FIRE_AT   = 10000;
	const int32 FIRE_LEAD = 100000;

	const int32 score = GDefconGameInstance->GetScore();

	FiringAccuracy eAccuracy = fa_lead;

	if(score < FIRE_AT)
	{
		eAccuracy = fa_wild;
	}
	else if(score < FIRE_LEAD)
	{
		// between firing at and leading.
		float f = NORM_(score, FIRE_AT, FIRE_LEAD);
		eAccuracy = FRAND < f ? fa_lead : fa_at;
	}

	CFPoint dir;
	ShortestDirection(from, GetPlayerShip().Position, dir);


	switch(eAccuracy)
	{
		case fa_wild:
			// Bullet is fired some random angle 
			// away from the player.
		{
			// firing angle is +/- 45 deg to zero
			// depending on player score.
			const float fa = SFRAND * MAP(score, 0, FIRE_AT, 45, 0);
			dir.Rotate(fa);
		}
			break;

		case fa_at:
			// Bullet is fired right at player.
			break;

		case fa_lead:
			// Lead the shot, taking player's velocity 
			// into account. Project where the player 
			// will be in some time from now, and 
			// compute a vector and speed to that spot.

			const float time = FRANDRANGE(0.75f, 1.25f);
			CFPoint pt = GetPlayerShip().Position + GetPlayerShip().GetVelocity() * time;

			if(pt.x < 0)
			{
				pt.x += GetWidth();
			}
			else if (pt.x >= GetWidth())
			{
				pt.x -= GetWidth();
			}

			const float speed = (1.0f / time) * ShortestDirection(from, pt, dir);
			pBullet->SetSpeed(speed);
			break;
	}

	pBullet->Orientation.Fwd = dir;
	// Place bullet just outside shooter, otherwise 
	// shooter will shoot himself. We could also solve 
	// this by making the shooter (or all members of 
	// the shooter's class) impervious to the bullet, 
	// but that would deny interesting friendly fire.
	pBullet->Position = from + pBullet->Orientation.Fwd * (2 * FMath::Max(obj.BboxRadius.x, obj.BboxRadius.y));

	gpAudio->OutputSound((Defcon::EAudioTrack)(soundid - 1 + (int32)Defcon::EAudioTrack::Bullet));

	Objects.Add(pBullet);

	return pBullet;
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


Defcon::IGameObject* UDefconPlayViewBase::FindHuman(float x) const
{
	// Return the uncarried humanoid nearest to a given point.

	Defcon::IGameObject* pObj = GetConstHumans().GetFirst();

	float bestX = 1.0e+10f;
	float aw = GetWidth();

	while(pObj != nullptr)
	{
		if(!static_cast<Defcon::CHuman*>(pObj)->IsBeingCarried() && pObj->Lifespan > 0.1f)
		{
			const float dist = Xdistance(pObj->Position.x, x);

			if(dist < HUMAN_WITHINRANGE && pObj->Position.y < GetTerrainElev(pObj->Position.x))
			{
				return pObj;
			}
		}
		pObj = pObj->GetNext();
	}
	return nullptr;
}


void UDefconPlayViewBase::ExplodeObject(Defcon::IGameObject* pObj)
{
	check(pObj != nullptr);

	// If object is already dying, then don't 
	// blow it up again. This fixes the problem 
	// where player got rewarded for killing
	// the same enemy multiple times.

	if(pObj->MarkedForDeath())
	{
		return;
	}

	pObj->Explode(Debris);

	// Not sure what the player ship being intact has to do with an object exploding
	//if(GetPlayerShip().IsAlive() && GetPlayerShip().IsSolid() /*&& Debris.Count() < 800*/)
	{
		const float em = pObj->GetExplosionMass();
		
		Defcon::EAudioTrack track;
		if(em >= 1.0f) 
		{
			const Defcon::EAudioTrack LargeExplosionSounds[] =
			{
				Defcon::EAudioTrack::Ship_exploding,
				Defcon::EAudioTrack::Ship_exploding2,
				Defcon::EAudioTrack::Ship_exploding2a,
				Defcon::EAudioTrack::Ship_exploding2b
			};

			track = LargeExplosionSounds[IRAND(array_size(LargeExplosionSounds))] ;
		}
		else if(em >= 0.3f)
		{
			track = Defcon::EAudioTrack::Ship_exploding_medium;
		}
		else
		{
			track = Defcon::EAudioTrack::Ship_exploding_small;
		}

		gpAudio->OutputSound(track);
	}

	if(pObj->GetType() != Defcon::EObjType::PLAYER)
	{
		IncreaseScore(pObj->GetPointValue(), SCORETIPS_SHOWENEMYPOINTS, &pObj->Position);
	}
}



void UDefconPlayViewBase::CheckIfObjectsGotHit(Defcon::CGameObjectCollection& objects)
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
	Defcon::IGameObject* pObj = Objects.GetFirst();

	Objects.ForEach([&](Defcon::IGameObject* ObjPtr)
	{
		if(!ObjPtr->IsInjurious())
		{
			return;
		}
		
		ObjPtr->GetInjurePt(InjurePt);

		Defcon::IGameObject* Obj2Ptr = objects.GetFirst();

		while(Obj2Ptr != nullptr)
		{
			if(!Obj2Ptr->CanBeInjured())
			{
				Obj2Ptr = Obj2Ptr->GetNext();
				continue;
			}

			if((ObjPtr->GetParentType() == Defcon::EObjType::BULLET) && ENEMIES_CANTHURTEACHOTHER)
			{
				// For non-players, bullets don't hurt.
				Obj2Ptr = Obj2Ptr->GetNext();
				continue;
			}
			else if(ObjPtr->GetType() == Defcon::EObjType::MINE && ENEMIES_MINESDONTHURT)
			{
				// For non-players, mines don't hurt.
				Obj2Ptr = Obj2Ptr->GetNext();
				continue;
			}

			GetMainAreaMapper().To(Obj2Ptr->Position, ScreenPos);

			if(!(ScreenPos.x > -20 && ScreenPos.x < DisplayWidth + 20))
			{
				Obj2Ptr = Obj2Ptr->GetNext();
				continue;
			}
				
			Bbox.Set(Obj2Ptr->Position);
			Bbox.Inflate(Obj2Ptr->BboxRadius);

			if(Bbox.PtInside(InjurePt) || ObjPtr->TestInjury(Bbox))
			{
				// Object has been hit! Ow!!!

				// todo: generalize code so that all objects have a RegisterImpact() method.
				// Can't use dynamic_cast, force all objects to be static castable to ILiveGameObject.

				auto pLiveObject = static_cast<Defcon::ILiveGameObject*>(Obj2Ptr);

				if(pLiveObject != nullptr)
				{
					const bool bKilled = pLiveObject->RegisterImpact(ObjPtr->GetCollisionForce());

					if(bKilled)
					{
						if(ObjPtr->GetType() == Defcon::EObjType::LASERBEAM)
						{
							if(Obj2Ptr->GetType() == Defcon::EObjType::HUMAN)
							{
								GDefconGameInstance->GetStats().FriendlyFireIncidents++;
							}
							else
							{
								GDefconGameInstance->GetStats().HostilesDestroyedByLaser++;
							}
						}

						ExplodeObject(Obj2Ptr);
					}
				}

				// Either way, the weapons fire object is toast.
				ObjPtr->MarkAsDead();

				// Skip testing targets, and continue with next bullet.
				break;
			}
			
			Obj2Ptr = Obj2Ptr->GetNext();
		}
	});
}



void UDefconPlayViewBase::OnPlayerShipDestroyed()
{
	// Player died; restart mission.

	if(!bArenaDying)
	{
		bArenaDying = true;

		auto pEvt = new Defcon::CRestartMissionEvent;
		pEvt->Init();

		pEvt->When = GameTime() + DESTROYED_PLAYER_LIFETIME * 2;
		Events.Add(pEvt);

		FadeAge = DESTROYED_PLAYER_LIFETIME * 2;
	}
}


void UDefconPlayViewBase::ShieldBonk(Defcon::IGameObject* pObj, float fForce)
{
	// Make a small visual splash to indicate that 
	// an object's shields have been struck by 
	// a certain force.

	const auto cby = Defcon::EColor::Gray;

	for(int32 I = 0; I < 10; I++)
	{
		auto FlakPtr = new Defcon::CFlak;

		FlakPtr->ColorbaseYoung = cby;
		FlakPtr->LargestSize = 4;
		FlakPtr->bFade = true;//bDieOff;

		FlakPtr->Position = pObj->Position;
		FlakPtr->Orientation = pObj->Orientation;

		CFPoint Direction;
		const double T = FRAND * TWO_PI;
		
		Direction.Set((float)cos(T), (float)sin(T));

		// Debris has at least the object's momentum.
		FlakPtr->Orientation.Fwd = pObj->Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		FlakPtr->Orientation.Fwd *= FRANDRANGE(20, 32);
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRANDRANGE(110, 140);

		FlakPtr->Orientation.Fwd.MulAdd(Direction, speed);

		Debris.Add(FlakPtr);
	}

	m_fRadarFritzed = FMath::Max(1.5f, fForce * 10);//MAX_RADARFRITZ;

	gpAudio->OutputSound(Defcon::EAudioTrack::Shieldbonk);
}


void UDefconPlayViewBase::ProcessWeaponsHits()
{
	CheckIfObjectsGotHit(Objects);
	CheckIfObjectsGotHit(Enemies);

	auto Mission = GDefconGameInstance->GetMission();

	if(Mission != nullptr && Mission->HumansInvolved())
	{
		CheckIfObjectsGotHit(GetHumans());
	}
}


void UDefconPlayViewBase::Hyperspace()
{
	// Make the player reappear elsewhere.
	// Destroy and recreate him so that it 
	// appears to hyper in.
	if(!GetPlayerShip().IsAlive())
	{
		return;
	}

	// Before deleting the player, note any carried humans
	// so we can attach them back.

#if 0
	Defcon::CGameObjectCollection tempset;
	Defcon::IGameObject* p2, *pObj = GetHumans().GetFirst();

	while(pObj != nullptr)
	{
		p2 = pObj->GetNext();
		CHuman& human = (CHuman&) *pObj;
		if(human.GetCarrier() == m_pPlayer)
		{
			human.Notify(EMessage::released, m_pPlayer);
			GetHumans().Detach(pObj);
			tempset.Add(pObj);
		}
			
		pObj = p2;
	}
#endif

	//const float fs = m_pPlayer->GetShieldStrength();
	//Objects.Delete(m_pPlayer);
	//m_pPlayer = new CPlayer;
	//m_pPlayer->SetShieldStrength(fs);

	// Randomize player ship position.
	GetPlayerShip().Position.Set(FRAND * ArenaWidth, (FRAND * 0.8f + 0.1f) * ArenaSize.Y);

	// todo: set player ship so that the body is hidden and it starts materializing

/*	m_pPlayer->Init(ArenaWidth);
	CFPoint pt = m_pPlayer->Position;
	pt.x -= w / 2;
	if(pt.x < 0)
		pt.x += w;
	pt.y = 0;
	m_coordMapper.ScrollTo(pt);
		
	m_pPlayer->MapperPtr = &m_coordMapper;
	Objects.Add(m_pPlayer);

	pObj = tempset.GetFirst();
	while(pObj != nullptr)
	{
		pObj->Notify(EMessage::takenaboard, m_pPlayer);
		pObj = pObj->GetNext();
	}
	GetHumans().Add(tempset);
	*/
	//check(tempset.Count() == 0);
}


void UDefconPlayViewBase::FireSmartbomb()
{
	if(GetPlayerShip().IsAlive())
	{
		GDefconGameInstance->GetStats().SmartbombsDetonated++;

		gpAudio->OutputSound(Defcon::EAudioTrack::Smartbomb);

		auto BombPtr = new Defcon::CSmartbomb;

		BombPtr->MapperPtr = &GetMainAreaMapper();
		BombPtr->Position = GetPlayerShip().Position;
		BombPtr->Range.Set(MainAreaSize.X, MainAreaSize.Y);
		BombPtr->Targets = &Enemies;
		BombPtr->Debris = &Debris;

		Blasts.Add(BombPtr);

		m_nFlashScreen = SMARTBOMB_MAX_FLASHSCREEN;
	}
}


namespace Defcon
{
	class CCreateMaterializationEvent : public CEvent
	{
		Defcon::FMaterializationParams Params;

		public:

			void InitMaterializationEvent(const Defcon::FMaterializationParams& InParams) { Params = InParams; }

			virtual void Do() override
			{
				auto Materialization = new Defcon::CMaterialization();

				Materialization->InitMaterialization(Params);

				GArena->AddDebris(Materialization);

				if(GArena->IsPointVisible(Params.P)) // todo: maybe check extents of materialization field i.e. if player gets even a partial glimpse
				{
					gpAudio->OutputSound(EAudioTrack::Ship_materialize);
				}
			}
	};
}


void UDefconPlayViewBase::SpecializeMaterialization(Defcon::FMaterializationParams& Params, Defcon::EObjType ObjectType)
{
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


void UDefconPlayViewBase::CreateEnemy(Defcon::EObjType EnemyType, const CFPoint& Where, float RelativeWhen, Defcon::EObjectCreationFlags Flags)
{
	// todo: we should take a parent type argument.

	auto TimeToDeploy = GameTime() + RelativeWhen;

	const auto MaterializationLifetime = ENEMY_BIRTHDURATION;

	if(0 != ((int32)Flags & (int32)Defcon::EObjectCreationFlags::Materializes))
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

		SpecializeMaterialization(Params, EnemyType);

		auto MaterializationEvent = new Defcon::CCreateMaterializationEvent;
		MaterializationEvent->InitMaterializationEvent(Params);
		MaterializationEvent->When = TimeToDeploy;

		TimeToDeploy += MaterializationLifetime;

		GDefconGameInstance->m_pMission->AddEvent(MaterializationEvent);
	}

	auto EventPtr = new Defcon::CCreateEnemyEvent;

	EventPtr->Init();

	EventPtr->EnemyType			= EnemyType;
	EventPtr->Where				= Where;
	EventPtr->bMissionTarget	= (0 != ((int32)Flags & (int32)Defcon::EObjectCreationFlags::IsMissionTarget));
	EventPtr->When				= TimeToDeploy;

	GDefconGameInstance->m_pMission->AddEvent(EventPtr);
}


Defcon::CEnemy* UDefconPlayViewBase::CreateEnemyNow(Defcon::EObjType EnemyType, const CFPoint& Where, Defcon::EObjectCreationFlags Flags)
{
	// Create an enemy immediately by executing its Do method and not putting the event into the event queue.

	auto EventPtr = new Defcon::CCreateEnemyEvent;

	EventPtr->Init();

	EventPtr->EnemyType      = EnemyType;
	EventPtr->Where          = Where;
	EventPtr->bMissionTarget = (0 != ((int32)Flags & (int32)Defcon::EObjectCreationFlags::IsMissionTarget));
	
	EventPtr->Do();

	return (Defcon::CEnemy*)GetEnemies().GetFirst();
}


void UDefconPlayViewBase::OnSelectEnemyToSpawn()
{
	SpawnedEnemyIndex = (SpawnedEnemyIndex + 1) % array_size(SpawnedEnemyTypes);

	FString Str = FString::Printf(TEXT("Enemy type %s chosen"), *Defcon::ObjectTypeManager.GetName(SpawnedEnemyTypes[SpawnedEnemyIndex]));

	AddMessage(Str, 0.25f);
}


void UDefconPlayViewBase::OnSpawnEnemy()
{
	// Debug tool, lets us spawn enemies on demand to test materialization, etc.

	auto pt = GetPlayerShip().Position;
	pt.x = WrapX(pt.x + 300 * SGN(GetPlayerShip().Orientation.Fwd.x));

	CreateEnemy(SpawnedEnemyTypes[SpawnedEnemyIndex], pt, 0.0f, 
		(Defcon::EObjectCreationFlags)((int32)Defcon::EObjectCreationFlags::Materializes | (int32)Defcon::EObjectCreationFlags::NotMissionTarget));
}


#undef MAX_RADARFRITZ
#undef FADE_DURATION	
#undef FADE_DURATION_NORMAL
#undef PLAYERSHIP_VMARGIN
#undef DURATION_IMPORTANT_MESSAGE


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
