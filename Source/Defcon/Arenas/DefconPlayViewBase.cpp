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


UDefconPlayViewBase* gpArena = nullptr;



float GameTime() { check(gpArena != nullptr); return UKismetSystemLibrary::GetGameTimeInSeconds(gpArena); }



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
		const float T = 1.0f - FMath::Max(0.0f, m_fFadeAge / FADE_DURATION_NORMAL);

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
	return gDefconGameInstance->GetHumans();
}


void UDefconPlayViewBase::DeleteAllObjects()
{
	m_enemies.DeleteAll(true);
	m_blasts. DeleteAll(true);
	m_debris. DeleteAll(true);
	m_objects.DeleteAll(true);

	m_events. DeleteAll();
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

	gpArena = this;

	m_bArenaDying = false;
	Daylon::Hide(Fader);

	ShipThrustSoundLoop = gpAudio->CreateLoopedSound(Defcon::snd_playership_thrust);
	WasShipUnderThrust = false;

	PlayAreaMain->ClearMessages();

	InitMapperAndTerrain();
	InitPlayerShip();

	PlayAreaMain->Humans     = &GetHumans();
	PlayAreaMain->Objects    = &m_objects; // todo: use Init method the way the rader widget does
	PlayAreaMain->Enemies    = &m_enemies;
	PlayAreaMain->Debris     = &m_debris;
	PlayAreaMain->Blasts     = &m_blasts;
	PlayAreaMain->ArenaSize  = ArenaSize;

	// Start the current mission.
	auto GI = gDefconGameInstance;

	if(!IsValid(GI))
	{
		return;
	}

	GI->InitMission(this);

	m_bHumansInMission = GI->GetMission()->HumansInvolved();



	PlayAreaRadar->Init(&GetPlayerShip(), MainAreaSize, (int32)ArenaWidth, &MainAreaMapper, &m_objects, &m_enemies);


	GetPlayerShip().BindToShieldValue([WeakThis = TWeakObjectPtr<UDefconPlayViewBase>(this)](const float& Value)
	{
		if(auto This = WeakThis.Get())
		{
			This->PlayAreaStats->UpdateShieldReadout(FMath::Clamp(Value, 0.0f, 1.0f));
		}
	});

	gDefconGameInstance->BindToSmartbombCount([WeakThis = TWeakObjectPtr<UDefconPlayViewBase>(this)](const int32& Value)
	{
		if(auto This = WeakThis.Get())
		{
			This->PlayAreaStats->UpdateSmartbombReadout(Value);
		}
	});


	PlayAreaMain  -> SetSafeToStart();
	PlayAreaRadar -> OnFinishActivating();

	GetPlayerShip().EnableInput();

	gpAudio->OutputSound(Defcon::EAudioTrack::snd_wave_start);
}


void UDefconPlayViewBase::OnDeactivate() 
{
	// We're being transitioned away from.
	// Empty all our object collections.

	LOG_UWIDGET_FUNCTION
	Super::OnDeactivate();

	PlayAreaMain->OnDeactivate();
	PlayAreaRadar->OnDeactivate();

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

	PlayerShip.WorldContextObject = this; // could probably do this in gameinstance

	PlayerShip.m_pMapper = &MainAreaMapper;
	PlayerShip.InitPlayer(ArenaWidth);

	// Position the ship at x=0, and halfway up.
	PlayerShip.m_pos.set(0.0f, ArenaSize.Y / 2);
	PlayerShip.FaceRight();
	
	PlayAreaMain->PlayerShipPtr = &PlayerShip;

	// Sprite installation for player ship happens in DefconPlayMainWidgetBase
	PlayerShip.SetIsAlive(true);

	PlayerShip.m_orient.fwd.x = 1.0f;

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

			if(Human->IsBeingCarried())
			{
				// Track highest human but not one that is too close to the top of the arena.
				if(Human->m_pos.y > pt.y && Human->m_pos.y < ArenaSize.Y - 150)
				{
					pt = Human->m_pos;
				}
			}
		});
	}

	auto& PlayerShip = GetPlayerShip();

	const auto Mission = static_cast<Defcon::CMilitaryMission*>(gDefconGameInstance->GetMission());

	if(pt.y != 0.0f)
	{
		// A human is being abducted.

		// Move player to where human is, plus some height so the lander can be targeted.

		PlayerShip.m_pos = pt;
		PlayerShip.m_pos.y += 100;

		// Move player horizontally by half the screen width, but facing the human.
		// If the player is still inside the stargate, adjust the distance until he's clear.
		float DistanceFactor = 0.4f;

		do
		{
			DistanceFactor += 0.1f;

			PlayerShip.m_pos.x = WrapX(pt.x - (PlayerShip.m_orient.fwd.x * GetDisplayWidth() * DistanceFactor));

		} while(Mission->PlayerInStargate());
	}
	else
	{
		// No humans are being abducted. 
		// For now, teleport the player randomly.

		do
		{
			PlayerShip.m_pos.set(WrapX(FRAND * ArenaWidth), (FRAND * 0.8f + 0.1f) * ArenaSize.Y);
		} while(Mission->PlayerInStargate());
	}

	// The SettlePlayer routine assumes the player ship is visible, and when it's not, 
	// it just slowly pans the arena until it is. We have to pan the arena immediately 
	// so that SettlePlayer doesn't have to do anything.

	// Get the player ship's screen location. If it's not at the margin (and it almost certainly won't be)
	// then we need to pan by the difference.

	CFPoint ScreenPt;
	MainAreaMapper.To(PlayerShip.m_pos, ScreenPt);

	const float kMargin = PLAYER_POSMARGIN;

	if(PlayerShip.m_orient.fwd.x > 0)
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
	MainAreaMapper.To(PlayerShip.m_pos, ScreenPt);

	if(PlayerShip.m_orient.fwd.x > 0)
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

	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlFwd,  MoveShipRightState.bActive, MoveShipRightState.fTimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlBack, MoveShipLeftState.bActive,  MoveShipLeftState.fTimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlUp,   MoveShipUpState.bActive,    MoveShipUpState.fTimeDown);
	PlayerShip.SetNavControl(Defcon::ILiveGameObject::ctlDown, MoveShipDownState.bActive,  MoveShipDownState.fTimeDown);
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


void UDefconPlayViewBase::IncreaseScore(size_t Points, bool bVis, const CFPoint* pPos)
{
	gDefconGameInstance->AdvanceScore((int32)Points);

	if(bVis)
	{
		// Add an upwards floating score readout to drift across the play field.
		check(pPos != nullptr);
#if 0
	// todo
		CTextDisplayer* pScore = new CTextDisplayer;
		pScore->m_pos = *pPos;
		pScore->m_pos.x += (FRAND - 0.5f) * 30;
		pScore->m_pos.y += 30;
		pScore->m_orient.fwd.set((FRAND - 0.5f)*100, 50.0f);
		pScore->Init((int)points);
		this->AddDebris(pScore);
#endif
	}
}


float UDefconPlayViewBase::GetTerrainElev(float X) const
{
	check(X >= 0 && X <= GetWidth());

	return Terrain->GetElev(X / GetWidth());
}


void UDefconPlayViewBase::ConcludeMission()
{
	// Wave can end. todo: handle via gameinstance?

	if(!m_bArenaDying)
	{
		m_bArenaDying = true;
		auto pEvt = new Defcon::CEndMissionEvent;
		pEvt->Init(this);
		//pEvt->m_what = CEvent::Type::endmission;
		pEvt->m_when = GameTime() + FADE_DURATION_NORMAL;
		m_events.Add(pEvt);

		m_fFadeAge = FADE_DURATION_NORMAL;

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

	void Draw(FPaintArguments& FrameBuffer, const I2DCoordMapper& CoordMapper)
	{
		CFPoint pt;
		CoordMapper.To(P, pt);
		const float HalfS = S / 2;

		Color.A = 1.0f - CLAMP(NORM_(Age, 0.0f, Lifetime), 0.0f, 1.0f);

		FrameBuffer.FillRect(pt.x - HalfS, pt.y - HalfS, pt.x + HalfS, pt.y + HalfS, Color);
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


	void Draw(FPaintArguments& FrameBuffer, const I2DCoordMapper& CoordMapper)
	{
		for(int32 y = 0; y < 2; y++)
		{
			for(int32 x = 0; x < 8; x++)
			{
				Particles[x][y].Draw(FrameBuffer, CoordMapper);
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
			m_type          = Defcon::ObjType::DESTROYED_PLAYER;
			m_creatorType   = Defcon::ObjType::PLAYER;
			m_fLifespan     = DESTROYED_PLAYER_LIFETIME;
			m_bMortal       = true;
			m_bCanBeInjured = false;
			m_fAge          = 0.0f;

			CreateSprite(Defcon::ObjType::DESTROYED_PLAYER);
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
			m_fAge += DeltaTime;

			if(m_fAge > DESTROYED_PLAYER_FLASH_DURATION)
			{
				// After the initial red-white flicker, stop showing the ship
				//Sprite->Hide();
				// Uninstall and reset the sprite; this will cause paint events to call Draw method.
				if(Sprite)
				{
					UninstallSprite();
					Sprite.Reset();

					gpAudio->OutputSound(Defcon::EAudioTrack::snd_ship_exploding2b);
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


		virtual void Draw(FPaintArguments& FrameBuffer, const I2DCoordMapper& CoordMapper)  override
		{
			// Our sprite will autodraw for the first 0.5 seconds, after that
			// we need to draw explosion debris.

			if(m_fAge <= DESTROYED_PLAYER_FLASH_DURATION)
			{
				return;
			}

			// Flash a few frames of increasingly transparent white.
			if(m_fAge < DESTROYED_PLAYER_FLASH_DURATION + DESTROYED_PLAYER_BLANK_DURATION)
			{
				// If we miss out on the fullscreen blank because of timing skips,
				// we can always use a frame count flag to force it.
				// It looks like the Xbox screen capture recording app can't see it, even though UE has a 60 fps frame rate.
				// Could be that the sudden change in screen content is too short to let the recorder save an I-frame.

				FLinearColor Color = C_WHITE;
				Color.A = NORM_(m_fAge, DESTROYED_PLAYER_FLASH_DURATION, DESTROYED_PLAYER_FLASH_DURATION + DESTROYED_PLAYER_BLANK_DURATION);
				Color.A = CLAMP(Color.A, 0.0f, 1.0f);
				
				FrameBuffer.FillRect(0.0f, 0.0f, FrameBuffer.GetWidth(), FrameBuffer.GetHeight(), Color);

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
			
				ParticleGroup.Draw(FrameBuffer, CoordMapper);
			}

			NumParticleGroupsToDraw = FMath::Min(NumParticleGroupsToDraw + 2, (int32)array_size(ParticleGroups));
		}

};


void UDefconPlayViewBase::DestroyPlayerShip()
{
	auto& PlayerShip = GetPlayerShip();

	AllStopPlayerShip();
	PlayerShip.EnableInput(false);
	PlayerShip.m_fLifespan = 0.0f;
	PlayerShip.SetIsAlive(false);

	// Start the player ship destruction sequence.
	// We do this with a special game object.

	auto pShip = new CDestroyedPlayerShip();

	pShip->InitDestroyedPlayer(PlayerShip.m_pos, PlayerShip.m_bboxrad, DESTROYED_PLAYER_PARTICLE_SPEED, DESTROYED_PLAYER_PARTICLE_MIN_LIFETIME, DESTROYED_PLAYER_PARTICLE_MAX_LIFETIME);

	pShip->m_pos        = PlayerShip.m_pos;
	pShip->m_orient.fwd = PlayerShip.m_orient.fwd;

	pShip->Sprite->FlipHorizontal = (pShip->m_orient.fwd.x < 0);

	pShip->InstallSprite();

	m_objects.Add(pShip);

	gpAudio->OutputSound(Defcon::snd_player_dying);

	PlayerShip.OnAboutToDie();
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

	bbox.set(PlayerShip.m_pos);
	bbox.inflate(PlayerShip.m_bboxrad);


	Defcon::IGameObject* pObj = m_objects.GetFirst();

	while(pObj != nullptr)
	{
		if(pObj->IsInjurious() && pObj->GetCreatorType() != Defcon::ObjType::PLAYER)
		{
			pObj->GetInjurePt(injurePt);

			const bool bHit = (bbox.ptinside(injurePt) || pObj->TestInjury(bbox));

			if(bHit)
			{
				// PlayerShip ship has been hit by a bullet.
				const bool bPlayerKilled = !gDefconGameInstance->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce());

				if(bPlayerKilled)
				{
					this->DestroyPlayerShip();
					//this->ExplodeObject(&PlayerShip);
				}
				else
				{
					this->ShieldBonk(&PlayerShip, pObj->GetCollisionForce());
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

	CFRect rObj, rPlayer(PlayerShip.m_pos);
	rPlayer.inflate(PlayerShip.m_bboxrad);
	
	Defcon::IGameObject* pObj = m_enemies.GetFirst();

	while(pObj != nullptr)
	{
		if(pObj->IsCollisionInjurious())
		{
			rObj.set(pObj->m_pos);
			rObj.inflate(pObj->m_bboxrad);

			if(rObj.intersect(rPlayer))
			{
				const bool bPlayerKilled = !gDefconGameInstance->GetGodMode() && PlayerShip.RegisterImpact(pObj->GetCollisionForce());

				if(bPlayerKilled)
				{
					this->DestroyPlayerShip();
					//this->ExplodeObject(&PlayerShip);
				}
				else
				{
					this->ShieldBonk(&PlayerShip, pObj->GetCollisionForce());
				}

				// Process collision outcome for object.
				if(pObj->CanBeInjured())
				{
					if(!bPlayerKilled)
					{
						this->ExplodeObject(pObj);
					}
					else
					{
						this->IncreaseScore(pObj->GetPointValue(), true, &pObj->m_pos);
						pObj->m_bMortal = true;
						pObj->m_fLifespan = 0.0f;
						pObj->OnAboutToDie();
						//this->ExplodeObject(pObj);
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
	m_debris.Add(p);
}


void UDefconPlayViewBase::CreateTerrain()
{
	SAFE_DELETE(Terrain);
	Terrain = new Defcon::CTerrain;
	Terrain->InitTerrain(-1, (int)GetWidth(), (int)GetHeight(), gDefconGameInstance->GetScore());

	PlayAreaMain->TerrainPtr = Terrain;
	PlayAreaRadar->SetTerrain(Terrain);
}


Defcon::CPlayer& UDefconPlayViewBase::GetPlayerShip() 
{
	return gDefconGameInstance->GetPlayerShip();
}


void UDefconPlayViewBase::UpdateGameObjects(float DeltaTime)
{
	if(!m_bArenaDying)
	{
		if(!gDefconGameInstance->Update(DeltaTime))
		{
			this->ConcludeMission();
		}
	}

	m_events.Process();


	if(m_fFadeAge > 0.0f)
	{
		m_fFadeAge -= DeltaTime;
	}

	if(m_bRunSlow)
	{
		DeltaTime *= 0.25f;
	}
	else if(ARENA_BOGDOWN)
	{
		if(m_enemies.Count() + m_objects.Count() + m_debris.Count() > 250)
		{
			if(m_enemies.Count() + m_objects.Count() < 500)
			{
				DeltaTime *= 0.8f;
			}
			else if(m_enemies.Count() + m_objects.Count() < 750)
			{
				DeltaTime *= 0.65f;
			}
			else //if(m_objects.Count() < 750)
			{
				DeltaTime *= 0.5f;
			}
		}
	}


	this->ProcessWeaponsHits();

	this->CheckPlayerCollided();
	this->CheckIfPlayerHit(m_objects);

	// Move and draw everyone.
	//m_virtualScreen.Clear(gGameColors.GetColor(0, (float)m_nFlashScreen/SMARTBOMB_MAX_FLASHSCREEN));

	if(m_nFlashScreen > 0)
	{
		m_nFlashScreen--;
	}

	const auto Mission = gDefconGameInstance->GetMission();
	
	const bool HumansInvolved = Mission != nullptr ? Mission->HumansInvolved() : false;

	// ------------------------------------------------------
	// Process objects (move, draw, delete).

	static size_t n = 0;
	const bool bDrawRadar = (n++ % 5 == 0);

	// Note: update only model data, not visual.
	Defcon::GameObjectProcessingParams gop;

	gop.UninstallSpriteIfObjectDeleted = true;
	gop.fElapsedTime	= DeltaTime;
	gop.fArenaWidth		= ArenaWidth;
	gop.fArenaHeight    = ArenaSize.Y;
	gop.pMapper			= &MainAreaMapper;
	gop.pvUser          = nullptr; // todo: we don't use this

	m_blasts.Process(gop);
	m_debris.Process(gop);

	if(HumansInvolved)
	{
		const auto NumHumansBefore = this->GetHumans().Count();
		this->GetHumans().Process(gop);
		const auto NumHumansNow = this->GetHumans().Count();

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

	gop.fnOnDeath = [this](Defcon::IGameObject* ObjPtr, void*)
	{
		const auto Mission = gDefconGameInstance->GetMission();
		
		if(!Mission->IsMilitary())
		{
			return;
		}

		const auto MilitaryMission = static_cast<Defcon::CMilitaryMission*>(Mission);

		if(ObjPtr->GetType() == Defcon::ObjType::LANDER)
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


	m_objects.Process(gop);

#if 0
	// Make visible any enemies that have finished materializing.
	// todo: would be nice to not do this every frame.
	gop.fnOnEvery = [](Defcon::IGameObject* pObj, void* pv)
	{
		auto EnemyPtr = (Defcon::CEnemy*)pObj;
		if(!EnemyPtr->IsMaterializing() && EnemyPtr->Sprite)
		{
			EnemyPtr->Sprite->Show();
		}
	};
#endif
	m_enemies.Process(gop);


	// ------------------------------------------------------


	const int32 w  = MainAreaSize.X;
	const int32 h  = MainAreaSize.Y;
	//const int32 hr = m_virtualRadarScreen.GetHeight();
	const int32 wp = (int32)ArenaWidth;// this->GetWidth();

	const float now = GameTime();

	auto& PlayerShip = GetPlayerShip();


	if(PlayerShip.IsAlive())
	{
		UpdatePlayerShipInputs();

		PlayerShip.Move(DeltaTime);

		// Constrain player vertically.
		PlayerShip.m_pos.y = CLAMP(PlayerShip.m_pos.y, PLAYERSHIP_VMARGIN, ArenaSize.Y - PLAYERSHIP_VMARGIN);

		SettlePlayer(DeltaTime);
		DoThrustSound(DeltaTime);

		// Handle wraparound onto planet.
		if(PlayerShip.m_pos.x < 0)
		{
			 PlayerShip.m_pos.x += ArenaWidth;
		}
		else if(PlayerShip.m_pos.x >= ArenaWidth)
		{
			PlayerShip.m_pos.x -= ArenaWidth;
		}


		if(PlayerShip.IsSolid())
		{
			// Things to do if player is solid.
			//static int mod = 0;

			// See if any carried humans can be debarked.
			if(/*mod++ % 5 == 0 && */PlayerShip.m_pos.y < 5.0f + this->GetTerrainElev(PlayerShip.m_pos.x))
			{
				if(PlayerShip.DebarkOnePassenger(this->GetHumans()))
				{
					this->IncreaseScore((size_t)HUMAN_VALUE_DEBARKED, true, &PlayerShip.m_pos);
				}
			}
			else
			{
				// See if any falling humans can be carried.
				if(HumansInvolved)
				{
					// todo: not sure why we're hit testing projected bboxes of player and humans, world space ought to be fine.
					CFPoint playerScreenPos;
					MainAreaMapper.To(PlayerShip.m_pos, playerScreenPos);

					CFRect rPlayer(playerScreenPos);
					rPlayer.inflate(PlayerShip.GetPickupRadBox());

					//Defcon::IGameObject* pObj = this->GetHumans().GetFirst();
					GetHumans().ForEachUntil([&](Defcon::IGameObject* pObj)
					//while(pObj != nullptr)
					{
						CFPoint humanScreenPos;
						MainAreaMapper.To(pObj->m_pos, humanScreenPos);

						if(rPlayer.intersect(CFRect(humanScreenPos)))
						{
							Defcon::CHuman& Human = *((Defcon::CHuman*)pObj);

							if(Human.IsFalling())
							{
								if(PlayerShip.EmbarkPassenger(pObj, this->GetHumans()))
								{
									this->IncreaseScore((size_t)HUMAN_VALUE_EMBARKED, true, &PlayerShip.m_pos);
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
			m_fPlayerRebirthClock -= fElapsedTime;
			m_fPlayerRebirthClock = FMath::Max(0, m_fPlayerRebirthClock);
			if(m_fPlayerRebirthClock == 0)
			{
				m_pPlayer = new CPlayer;
				m_coordMapper.From(
					CFPoint(m_virtualScreen.GetWidth()/2, 
					m_virtualScreen.GetHeight()/2), 
					m_pPlayer->m_pos);
				m_pPlayer->Init((float)wp);
				m_pPlayer->m_pMapper = &m_coordMapper;
				m_objects.Add(m_pPlayer);
				
				m_fPlayerRebirthClock = PLAYER_REBIRTH_DELAY;
			}
		}
		else
#endif
		{
			// Player died; restart mission.
			if(!m_bArenaDying)
			{
				m_bArenaDying = true;
				auto pEvt = new Defcon::CRestartMissionEvent;
				pEvt->Init(this);

				pEvt->m_when = now + FADE_DURATION_NORMAL;
				m_events.Add(pEvt);

				m_fFadeAge = FADE_DURATION_NORMAL;
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
		InputStatePtr->fTimeDown = GameTime(); 
	}
}


void UDefconPlayViewBase::OnNavEvent(ENavigationKey Key)
{
	// debugging only
	const float Direction = (Key == ENavigationKey::Left ? -1.0f : 1.0f);

	CFPoint pt = MainAreaMapper.GetOffset();
	pt.x += Direction * 100.0f;
	pt.x = FMath::Wrap(pt.x, 0.0f, MainAreaMapper.m_planetCircumference);
	//pt.x = (int32)(pt.x + MainAreaMapper.m_planetCircumference) % (int32)MainAreaMapper.m_planetCircumference;
	MainAreaMapper.ScrollTo(pt);

	// Move the player to keep pace.
	//PlayerShip.m_pos.x = FMath::Wrap(pt.x + 1920/2, 0.0f, MainAreaMapper.m_planetCircumference);
}


void UDefconPlayViewBase::OnPawnWeaponEvent(EDefconPawnWeaponEvent Event, bool Active)
{
	switch(Event)
	{
		case EDefconPawnWeaponEvent::FireLaser: 

			if(GetPlayerShip().IsAlive())
			{
				GetPlayerShip().FireLaserWeapon(m_objects);

				if(true/*BRAND*/)
					gpAudio->OutputSound(Defcon::EAudioTrack::snd_laserfire);
				else
					gpAudio->OutputSound(Defcon::snd_laserfire_alt);
			}

		break;


		case EDefconPawnWeaponEvent::FireSmartbomb: 

			if(GetPlayerShip().IsAlive() && gDefconGameInstance->AcquireSmartBomb())
			{
				FireSmartbomb();
			}

		break;
	}
}


bool UDefconPlayViewBase::IsPointVisible(const CFPoint& pt) const
{
	// Given an arena location, convert to main 
	// framebuffer space and see if within bounds.

	CFPoint pt2;
	MainAreaMapper.To(pt, pt2);

	return (pt2.x >= 0 && pt2.x < GetDisplayWidth());
}


typedef enum { fa_wild, fa_at, fa_lead } FiringAccuracy;


float UDefconPlayViewBase::Direction(const CFPoint& posA, const CFPoint& posB, CFPoint& result) const
{
	// Return direction to achieve shortest travel 
	// time from point A to point B. The distance 
	// is returned also.

	CFPoint a(posA), b(posB);

	const float w = this->GetWidth();

	const float fFromTail1 = w - a.x;
	const float fFromTail2 = w - b.x;

	const float d0 = ABS(b.x - a.x);
	const float d1 = a.x + fFromTail2;
	const float d2 = b.x + fFromTail1;

	const float xdist = FMath::Min(d0, FMath::Min(d1, d2));


	if(d0 == xdist)
	{
		result = b - a;
	}
	else if(d1 == xdist)
	{
		// B behind origin, A past it.
		result = b;
		result.x -= w;
		result -= a;
	}
	else if(d2 == xdist)
	{
		// A behind origin, B past it.
		result = a;
		result.x -= w;
		result = b - result;
	}

	const float dist = result.length();
	check(xdist <= w/2);
	check(ABS(result.x) <= w/2);
	result.normalize();

	return dist;
}


void UDefconPlayViewBase::Lerp(const CFPoint& pt1, const CFPoint& pt2, CFPoint& result, float t) const
{
	// Lerp arena coords pt1 and pt2.
	// lerp locations lie along the shortest line 
	// between pt1 and pt2.

	CFPoint dir;
	float len = this->Direction(pt1, pt2, dir);

	dir *= len * t;
	result = pt1 + dir;

	WRAP(result.x, 0, this->GetWidth());

	/*if(result.x < 0)
	{
		result.x += this->GetWidth();
	}
	else if(result.x >= this->GetWidth())
	{
		result.x -= this->GetWidth();
	}*/
}


float UDefconPlayViewBase::WrapX(float x) const
{
	const auto w = this->GetWidth();

	if(x > w) return x - w;
	if(x < 0) return x + w;

	return x;
}


void UDefconPlayViewBase::LayMine(Defcon::IGameObject& obj, const CFPoint& from, int, int)
{
	auto p = new Defcon::CMine;

	p->InstallSprite();
	p->m_pos = obj.m_pos - obj.m_orient.fwd * (2 * FMath::Max(obj.m_bboxrad.x, obj.m_bboxrad.y));

	m_objects.Add(p);
}


Defcon::IBullet* UDefconPlayViewBase::FireBullet(Defcon::IGameObject& obj, const CFPoint& from, int soundid, int)
{
	//check(m_pPlayer != nullptr);
	Defcon::IBullet* pBullet;

	if(obj.GetType() == Defcon::ObjType::GUPPY)
	{
		pBullet = new Defcon::CThinBullet;
	}
	else
	{
		pBullet = new Defcon::CBullet;
	}

		
	//auto pBullet = new Defcon::CBullet;
	pBullet->SetCreatorType(obj.GetType());
	pBullet->WorldContextObject = obj.WorldContextObject;
	pBullet->InstallSprite();

	// Scores at which to switch firing style.
	const size_t FIRE_AT   = 10000;
	const size_t FIRE_LEAD = 100000;

	const size_t score = gDefconGameInstance->GetScore();

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
	this->Direction(from, GetPlayerShip().m_pos, dir);


	switch(eAccuracy)
	{
		case fa_wild:
			// Bullet is fired some random angle 
			// away from the player.
		{
			// firing angle is +/- 45 deg to zero
			// depending on player score.
			const float fa = SFRAND * MAP(score, 0, FIRE_AT, 45, 0);
			dir.rotate(fa);
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
			CFPoint pt = GetPlayerShip().m_pos + GetPlayerShip().GetVelocity() * time;

			if(pt.x < 0)
			{
				pt.x += this->GetWidth();
			}
			else if (pt.x >= this->GetWidth())
			{
				pt.x -= this->GetWidth();
			}

			const float speed = (1.0f / time) * this->Direction(from, pt, dir);
			pBullet->SetSpeed(speed);
			break;
	}

	pBullet->m_orient.fwd = dir;
	// Place bullet just outside shooter, otherwise 
	// shooter will shoot himself. We could also solve 
	// this by making the shooter (or all members of 
	// the shooter's class) impervious to the bullet, 
	// but that would deny interesting friendly fire.
	pBullet->m_pos = from + pBullet->m_orient.fwd * (2 * FMath::Max(obj.m_bboxrad.x, obj.m_bboxrad.y));

	gpAudio->OutputSound((Defcon::EAudioTrack)(soundid - 1 + (int32)Defcon::EAudioTrack::snd_bullet));

	m_objects.Add(pBullet);

	return pBullet;
}


float UDefconPlayViewBase::Xdistance(float x1, float x2) const
{
	// Return the shortest distance between two 
	// horizontal locations on the arena.
	
	const float aw = this->GetWidth();

	const float fFromTail1 = aw - x1;
	const float fFromTail2 = aw - x2;

	const float d0 = ABS(x2 - x1);
	const float d1 = x1 + fFromTail2;
	const float d2 = x2 + fFromTail1;

	return FMath::Min(d0, FMath::Min(d1, d2));
}


Defcon::IGameObject* UDefconPlayViewBase::FindHuman(float x) const
{
	// Return the uncarried humanoid nearest to a given point.

	Defcon::IGameObject* pObj = GetConstHumans().GetFirst();

	float bestX = 1.0e+10f;
	float aw = this->GetWidth();

	while(pObj != nullptr)
	{
		if(!((Defcon::CHuman*)pObj)->IsBeingCarried() && pObj->m_fLifespan > 0.1f)
		{
			const float dist = this->Xdistance(pObj->m_pos.x, x);

			if(dist < HUMAN_WITHINRANGE && pObj->m_pos.y < this->GetTerrainElev(pObj->m_pos.x))
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

	pObj->Explode(m_debris);

	if(GetPlayerShip().IsAlive() && GetPlayerShip().IsSolid() /*&& m_debris.Count() < 800*/)
	{
		//gpAudio->OutputSound(Defcon::snd_ship_exploding);

		const float em = pObj->GetExplosionMass();

		
		Defcon::EAudioTrack track;
		if(em >= 1.0f) 
		{
			const Defcon::EAudioTrack LargeExplosionSounds[] =
			{
				Defcon::EAudioTrack::snd_ship_exploding,
				Defcon::EAudioTrack::snd_ship_exploding2,
				Defcon::EAudioTrack::snd_ship_exploding2a,
				Defcon::EAudioTrack::snd_ship_exploding2b
			};

			track = LargeExplosionSounds[IRAND(array_size(LargeExplosionSounds))] ;
		}
		else if(em >= 0.3f)
		{
			track = Defcon::snd_ship_exploding_medium;
		}
		else
		{
			track = Defcon::snd_ship_exploding_small;
		}

		gpAudio->OutputSound(track);
	}

	if(pObj->GetType() != Defcon::ObjType::PLAYER)
	{
		this->IncreaseScore(pObj->GetPointValue(), SCORETIPS_SHOWENEMYPOINTS, &pObj->m_pos);
	}
}



void UDefconPlayViewBase::CheckIfObjectsGotHit(Defcon::CGameObjectCollection& objects)
{
	// See if anything got hit, and if so, 
	// mark them for injury or death.

	// Cross ref all injurious objects (weapons fire)
	// with all injurable objects (such as player and enemies).


	CFRect bbox;
	CFPoint injurePt;
	CFPoint screenPos;

	// The debris is a seperate game object collection
	// so that we can build it up without disturbing 
	// the arena's object collection, which is necessary 
	// since we are searching through it.
	//CGameObjectCollection	debris;
	const int32 sw = GetDisplayWidth();

	// Loop through the weapons fire, 
	// then, for each bullet, loop through the targets.
	Defcon::IGameObject* pObj = m_objects.GetFirst();

	m_objects.ForEach([&](Defcon::IGameObject* pObj)
	//{
	//while(pObj != nullptr)
	{
		if(!pObj->IsInjurious())
		{
			//pObj = pObj->GetNext();
			//continue;
			return;
		}
		
		pObj->GetInjurePt(injurePt);

		Defcon::IGameObject* pObj2 = objects.GetFirst();

		while(pObj2 != nullptr)
		{
			if(!pObj2->CanBeInjured())
			{
				pObj2 = pObj2->GetNext();
				continue;
			}
			/*			
			// Player ship is not in the objects list so this block would never run
			if(pObj2 == &GetPlayerShip())
			{
				// If the target is the player ship, 
				// and the bullet is one of its lasers, 
				// then don't check. We have this suicide 
				// problem where a laser beam wraps around
				// and hits us (duh).
				if(pObj->GetCreatorType() == Defcon::ObjType::PLAYER)
				{
					pObj2 = pObj2->GetNext();
					continue;
				}
			}
			else*/ if((pObj->GetParentType() == Defcon::ObjType::BULLET) && ENEMIES_CANTHURTEACHOTHER)
			{
				// For non-players, bullets don't hurt.
				pObj2 = pObj2->GetNext();
				continue;
			}
			else if(pObj->GetType() == Defcon::ObjType::MINE && ENEMIES_MINESDONTHURT)
			{
				// For non-players, mines don't hurt.
				pObj2 = pObj2->GetNext();
				continue;
			}

			GetMainAreaMapper().To(pObj2->m_pos, screenPos);

			if(!(screenPos.x > -20 && screenPos.x < sw + 20))
			{
				pObj2 = pObj2->GetNext();
				continue;
			}
				
			bbox.set(pObj2->m_pos);
			bbox.inflate(pObj2->m_bboxrad);

			const bool bHit = (bbox.ptinside(injurePt) || pObj->TestInjury(bbox));

			if(bHit)
			{
				// Object has been hit! Ow!!!

				// todo: generalize code so 
				// that all objects have a 
				// RegisterImpact() method.
				/*
				// Player ship not in object list so this block never runs
				if(pObj2 == &GetPlayerShip())
				{
					const bool bPlayerKilled = GetPlayerShip().RegisterImpact(pObj->GetCollisionForce());

					if(bPlayerKilled)
						this->ExplodeObject(&GetPlayerShip());
					else
						this->ShieldBonk(&GetPlayerShip(), pObj->GetCollisionForce());
				}
				else*/
				{
					// todo: if we can't use dynamic_cast, then we force all objects to be static castable to ILiveGameObject.
					auto pLiveObject = static_cast<Defcon::ILiveGameObject*>(pObj2);// nullptr;// todo: dynamic_cast<ILiveGameObject*>(pObj2);

					if(pLiveObject != nullptr)
					{
						const bool bKilled = pLiveObject->RegisterImpact(pObj->GetCollisionForce());

						if(bKilled)
						{
							this->ExplodeObject(pObj2);
						}
					}
				}

				// Either way, bullet is toast.
				pObj->MarkAsDead();
				// Skip testing targets, and 
				// continue with next bullet.
				break;
			}
			
			pObj2 = pObj2->GetNext();
		}
		
		//pObj = pObj->GetNext();
	});
}



void UDefconPlayViewBase::OnPlayerShipDestroyed()
{
	// Player died; restart mission.

	if(!m_bArenaDying)
	{
		m_bArenaDying = true;

		auto pEvt = new Defcon::CRestartMissionEvent;
		pEvt->Init(this);

		pEvt->m_when = GameTime() + DESTROYED_PLAYER_LIFETIME * 2;
		m_events.Add(pEvt);

		m_fFadeAge = DESTROYED_PLAYER_LIFETIME * 2;
	}
}


void UDefconPlayViewBase::ShieldBonk(Defcon::IGameObject* pObj, float fForce)
{
	// Make a small visual splash to indicate that 
	// an object's shields have been struck by 
	// a certain force.

	const int cby = Defcon::CGameColors::gray;

	for(size_t i = 0; i < 10; i++)
	{
		auto pFlak = new Defcon::CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->m_pos = pObj->m_pos;
		pFlak->m_orient = pObj->m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = pObj->m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->m_orient.fwd.muladd(dir, speed);

		m_debris.Add(pFlak);
	}
	m_fRadarFritzed = FMath::Max(1.5f, fForce*10);//MAX_RADARFRITZ;

	gpAudio->OutputSound(Defcon::snd_shieldbonk);
}



void UDefconPlayViewBase::ProcessWeaponsHits()
{
	this->CheckIfObjectsGotHit(m_objects);
	this->CheckIfObjectsGotHit(m_enemies);

	auto Mission = gDefconGameInstance->GetMission();

	if(Mission != nullptr && Mission->HumansInvolved())
	{
		this->CheckIfObjectsGotHit(this->GetHumans());
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
	Defcon::IGameObject* p2, *pObj = this->GetHumans().GetFirst();

	while(pObj != nullptr)
	{
		p2 = pObj->GetNext();
		CHuman& human = (CHuman&) *pObj;
		if(human.GetCarrier() == m_pPlayer)
		{
			human.Notify(Message::released, m_pPlayer);
			this->GetHumans().Detach(pObj);
			tempset.Add(pObj);
		}
			
		pObj = p2;
	}
#endif

	//const float fs = m_pPlayer->GetShieldStrength();
	//m_objects.Delete(m_pPlayer);
	//m_pPlayer = new CPlayer;
	//m_pPlayer->SetShieldStrength(fs);

	// Randomize player ship position.
	GetPlayerShip().m_pos.set(FRAND * ArenaWidth, (FRAND * 0.8f + 0.1f) * ArenaSize.Y);

	// todo: set player ship so that the body is hidden and it starts materializing

/*	m_pPlayer->Init(ArenaWidth);
	CFPoint pt = m_pPlayer->m_pos;
	pt.x -= w / 2;
	if(pt.x < 0)
		pt.x += w;
	pt.y = 0;
	m_coordMapper.ScrollTo(pt);
		
	m_pPlayer->m_pMapper = &m_coordMapper;
	m_objects.Add(m_pPlayer);

	pObj = tempset.GetFirst();
	while(pObj != nullptr)
	{
		pObj->Notify(Message::takenaboard, m_pPlayer);
		pObj = pObj->GetNext();
	}
	this->GetHumans().Add(tempset);
	*/
	//check(tempset.Count() == 0);
}


void UDefconPlayViewBase::FireSmartbomb()
{
	if(GetPlayerShip().IsAlive() )
	{
		gpAudio->OutputSound(Defcon::snd_smartbomb);

		auto pBomb = new Defcon::CSmartbomb;

		pBomb->m_pMapper = &GetMainAreaMapper();
		pBomb->m_pos = GetPlayerShip().m_pos;
		pBomb->m_range.set(MainAreaSize.X, MainAreaSize.Y);
		pBomb->m_pTargets = &m_enemies;
		pBomb->m_pDebris = &m_debris;
		pBomb->m_pArena = this;

		m_blasts.Add(pBomb);

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

				gpArena->AddDebris(Materialization);

				if(gpArena->IsPointVisible(Params.P)) // todo: maybe check extents of materialization field i.e. if player gets even a partial glimpse
				{
					gpAudio->OutputSound(snd_ship_materialize);
				}
			}
	};
}


void UDefconPlayViewBase::SpecializeMaterialization(Defcon::FMaterializationParams& Params, Defcon::ObjType ObjectType)
{
	switch(ObjectType)
	{
		case Defcon::ObjType::SWARMER:
			Params.NumParticles       = 50;
			Params.StartingRadiusMin *= 0.5f;
			Params.StartingRadiusMax *= 0.5f;
			Params.ParticleSizeMin    = 2.0f;
			Params.ParticleSizeMax    = 5.0f;
			Params.AspectRatio        = 1.0f;
			Params.Colors             = { C_MAGENTA, C_RED, C_ORANGE };
			break;

		case Defcon::ObjType::GUPPY:
			Params.NumParticles       = 75;
			Params.StartingRadiusMin *= 0.75f;
			Params.StartingRadiusMax *= 0.75f;
			Params.ParticleSizeMin    = 2.5f;
			Params.ParticleSizeMax    = 5.5f;
			Params.AspectRatio        = 0.75f;
			Params.Colors             = { C_MAGENTA, C_MAGENTA, C_MAGENTA };
			break;

		case Defcon::ObjType::POD:
			Params.Colors             = { C_RED, C_MAGENTA, C_BLUE };
			break;

		case Defcon::ObjType::BOMBER:
			Params.Colors             = { C_BLUE, C_MAGENTA };
			break;

		case Defcon::ObjType::REFORMER:
			Params.Colors             = { C_RED, C_DARKRED, C_DARKER, C_DARKYELLOW };
			break;

		case Defcon::ObjType::FIREBOMBER_TRUE:
		case Defcon::ObjType::BOUNCER_TRUE:
			Params.NumParticles       = 150;
			Params.Colors             = { C_YELLOW };
			break;

		case Defcon::ObjType::FIREBOMBER_WEAK:
		case Defcon::ObjType::BOUNCER_WEAK:
			Params.NumParticles       = 150;
			Params.Colors             = { C_WHITE, C_LIGHTYELLOW, C_YELLOW };
			break;

		case Defcon::ObjType::HUNTER:
			Params.NumParticles       = 150;
			Params.Colors             = { C_LIGHTYELLOW, C_YELLOW, C_ORANGE };
			break;

		case Defcon::ObjType::BIGRED:
			Params.Colors             = { C_RED, C_ORANGE };
			break;

		case Defcon::ObjType::LANDER:
			Params.Colors             = { C_YELLOW, C_GREEN };
			break;

		case Defcon::ObjType::BAITER:
			Params.Colors             = { C_YELLOW, C_GREEN };
			Params.StartingRadiusMin *= 3.0f;
			Params.StartingRadiusMax *= 3.0f;
			Params.AspectRatio        = 4.0f;
			break;

		case Defcon::ObjType::MUNCHIE:
			Params.NumParticles       = 80;
			Params.StartingRadiusMin *= 0.75f;
			Params.StartingRadiusMax *= 0.75f;
			Params.ParticleSizeMin    = 2.5f;
			Params.ParticleSizeMax    = 5.5f;
			Params.Colors             = { C_GREEN, C_LIGHTGREEN };
			break;
	}
}


void UDefconPlayViewBase::CreateEnemy(Defcon::ObjType EnemyType, const CFPoint& where, float RelativeWhen, bool bMaterializes, bool bTarget)
{
	auto TimeToDeploy = GameTime() + RelativeWhen;

	const auto MaterializationLifetime = ENEMY_BIRTHDURATION;

	if(bMaterializes)
	{
		Defcon::FMaterializationParams Params;

		Params.Lifetime          = MaterializationLifetime;
		Params.P                 = where;
		Params.NumParticles      = 100;
		Params.OsMin             = 0.0f;
		Params.OsMax             = 1.0f;
		Params.ParticleSizeMin   = 3.0f;
		Params.ParticleSizeMax   = 6.0f;
		Params.StartingRadiusMin = ENEMY_BIRTHDEBRISDIST * 0.1f;
		Params.StartingRadiusMax = ENEMY_BIRTHDEBRISDIST;
		Params.AspectRatio       = 1.5f;
		Params.Colors            = { C_WHITE, C_RED, C_YELLOW, C_ORANGE, C_LIGHTYELLOW };
		Params.TargetBoxRadius.set(0.0f, 0.0f);

		SpecializeMaterialization(Params, EnemyType);

		auto MaterializationEvent = new Defcon::CCreateMaterializationEvent;
		MaterializationEvent->InitMaterializationEvent(Params);
		MaterializationEvent->m_when = TimeToDeploy;

		TimeToDeploy += MaterializationLifetime;

		gDefconGameInstance->m_pMission->AddEvent(MaterializationEvent);
	}

	auto p = new Defcon::CCreateEnemyEvent;
	p->Init(this);

	p->m_objtype			= EnemyType;
	p->m_where				= where;
	//p->m_bMaterializes		= false;//bMaterializes;
	p->m_bMissionTarget		= bTarget;
	p->m_when				= TimeToDeploy;

	gDefconGameInstance->m_pMission->AddEvent(p);
}


Defcon::CEnemy* UDefconPlayViewBase::CreateEnemyNow(Defcon::ObjType kind, const CFPoint& where, bool bMaterializes, bool bTarget)
{
	// Create an enemy immediately by executing its Do method and not putting the event into the event queue.
	auto* p = new Defcon::CCreateEnemyEvent;
	p->Init(this);
	p->m_objtype = kind;
	p->m_where = where;
	//p->m_bMaterializes = bMaterializes;
	p->m_bMissionTarget = bTarget;
	
	p->Do();
	return (Defcon::CEnemy*)GetEnemies().GetFirst();
}


void UDefconPlayViewBase::OnSpawnEnemy()
{
	const Defcon::ObjType Types[] =
	{
		Defcon::ObjType::LANDER,
		Defcon::ObjType::HUNTER,
		Defcon::ObjType::GUPPY,
		Defcon::ObjType::BOMBER,
		Defcon::ObjType::POD,
		Defcon::ObjType::SWARMER,
		Defcon::ObjType::BAITER,
		Defcon::ObjType::PHRED,
		Defcon::ObjType::BIGRED,
		Defcon::ObjType::MUNCHIE,
		Defcon::ObjType::FIREBOMBER_TRUE,
		Defcon::ObjType::FIREBOMBER_WEAK,
		Defcon::ObjType::DYNAMO,
		Defcon::ObjType::SPACEHUM,
		Defcon::ObjType::REFORMER,
		Defcon::ObjType::GHOST,
		Defcon::ObjType::BOUNCER_TRUE,
		Defcon::ObjType::BOUNCER_WEAK
	};

	static int Index = 0;

	auto pt = GetPlayerShip().m_pos;
	pt.x = WrapX(pt.x + 300 * SGN(GetPlayerShip().m_orient.fwd.x));

	CreateEnemy(Types[Index], pt, 0.0f, true, false);

	Index = (Index + 1) % array_size(Types);
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