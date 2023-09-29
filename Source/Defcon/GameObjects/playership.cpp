// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// playership.cpp


#include "playership.h"

#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"

#include "Common/util_core.h"
#include "Common/util_str.h"
#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Common/gamma.h"
#include "GameObjects/Enemies/enemies.h"
#include "flak.h"
#include "bmpdisp.h"
#include "human.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconLogging.h"



#define DEBUG_MODULE      1

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Defcon::CPlayerShip::CPlayerShip()
{
	ParentType = Type;
	Type       = EObjType::PLAYER;

	// Start off invincible, switch after birth finished.
	bMortal       = false;
	bCanBeInjured = true;

	LaserWeapon.MountOnto(*this, CFPoint(0,0));
	LaserWeapon.SetEmissionPt(CFPoint(12, -6));

	SecondaryLaserWeapon.MountOnto(*this, CFPoint(0,0));
	SecondaryLaserWeapon.SetEmissionPt(CFPoint(0, -12));

	SetShieldStrength(1.0f);

	CreateSprite(Type);

	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius = Info.Size * 0.25f;

	// Make our "pickup human" bboxrad more generous than hitbox.
	PickupBboxRadius = Info.Size * 0.4f;

	SmartbombsLeft.Bind(EMessageEx::SmartbombCountChanged);
	SmartbombsLeft.Set(3);

	DoubleGunsActive.Bind(EMessageEx::DoubleGunsActivated);
	DoubleGunsActive.Set(false);

	DoubleGunsLeft.Bind(EMessageEx::DoubleGunsLevelChanged);
	DoubleGunsLeft.Set(1.0f);

	InvincibilityLeft.Bind(EMessageEx::InvincibilityLevelChanged);
	InvincibilityLeft.Set(1.0f);

	InvincibilityActive.Bind(EMessageEx::InvincibilityActivated);
	InvincibilityActive.Set(false);
}


void Defcon::CPlayerShip::InitPlayerShip()
{
	Drag      = PLAYER_DRAG;
	MaxThrust = PLAYER_MAXTHRUST;
	Mass      = PLAYER_MASS;

	DeactivateDoubleGuns();
	DeactivateInvincibility();
}


Defcon::CPlayerShip::~CPlayerShip() {}


void Defcon::CPlayerShip::SetIsAlive(bool b)
{
	ILiveGameObject::SetIsAlive(b);

	EnableInput(b);

	// Play arena activate/deactivate take care of installing/uninstalling sprite
	// required by arena transition.

	if(Sprite)
	{
		Sprite->Show(b);
	}

	if(b)
	{
		SetShieldStrength(1.0f);
	}
	else
	{
	}
}


void Defcon::CPlayerShip::OnAboutToDie()
{
	// UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	CGameObjectCollection& Humans = GArena->GetHumans();

	Humans.ForEach([this](IGameObject* pObj)
	{
		CHuman& Human = (CHuman&)*pObj;
		if(Human.GetCarrier() == this)
		{
			pObj->Notify(EMessage::CarrierKilled, this);
		}
	});
}


bool Defcon::CPlayerShip::ToggleDoubleGuns() 
{
	DoubleGunsActive.Set(!DoubleGunsActive.Get());
	return (DoubleGunsLeft.Get() > 0.0f);
}


void Defcon::CPlayerShip::DeactivateDoubleGuns() 
{
	DoubleGunsActive.Set(false); 
}


void Defcon::CPlayerShip::AddDoubleGunPower(float Amount)
{
	DoubleGunsLeft.Set(DoubleGunsLeft.Get() + Amount, (Amount == 0.0f));
}


bool Defcon::CPlayerShip::ToggleInvincibility() 
{
	InvincibilityActive.Set(!InvincibilityActive.Get()); 
	const bool Ret = (InvincibilityLeft.Get() > 0.0f);

	if(Sprite)
	{
		Sprite->SetAtlas(GGameObjectResources.Get(Ret && InvincibilityActive.Get() ? EObjType::INVINCIBLE_PLAYER : EObjType::PLAYER).Atlas->Atlas);
	}

	return Ret;
}


void Defcon::CPlayerShip::DeactivateInvincibility() 
{
	InvincibilityActive.Set(false);

	if(Sprite)
	{
		Sprite->SetAtlas(GGameObjectResources.Get(EObjType::PLAYER).Atlas->Atlas);
	}
}


void Defcon::CPlayerShip::AddInvincibility(float Amount)
{
	const float NewAmt = FMath::Min(1.0f, InvincibilityLeft.Get() + Amount);

	InvincibilityLeft.Set(NewAmt, (Amount == 0.0f));

	if(Sprite)
	{
		Sprite->SetAtlas(GGameObjectResources.Get(InvincibilityLeft.Get() > 0.0f && InvincibilityActive.Get() ? EObjType::INVINCIBLE_PLAYER : EObjType::PLAYER).Atlas->Atlas);
	}
}


void Defcon::CPlayerShip::AddSmartBombs(int32 Amount) 
{
	SmartbombsLeft.Set(SmartbombsLeft.Get() + Amount, (Amount == 0)); 
}


bool Defcon::CPlayerShip::AcquireSmartBomb()
{
	if(SmartbombsLeft.Get() > 0)
	{
		SmartbombsLeft.Set(SmartbombsLeft.Get() - 1);
		return true;
	}

	return false;
}


bool Defcon::CPlayerShip::EmbarkPassenger(IGameObject* pObj, CGameObjectCollection& humans)
{
	if(MarkedForDeath())
	{
		return false;
	}

	pObj->Notify(EMessage::TakenAboard, this);


	//UE_LOG(LogGame, Log, TEXT("Human picked up by player"));

	return true;
}


bool Defcon::CPlayerShip::DebarkOnePassenger(CGameObjectCollection& Humans)
{
	bool Result = false;

	Humans.ForEachUntil([&](IGameObject* Obj)
	{
		auto Human = static_cast<CHuman*>(Obj);

		if(Human->GetCarrier() == this)
		{
			// UE_LOG(LogGame, Log, TEXT("Human released by player"));
			Human->Notify(EMessage::Released, this);
			Result = true;
			return false;
		}
		return true;
	});

	return Result;
}


void Defcon::CPlayerShip::Tick(float DeltaTime)
{
	ILiveGameObject::Tick(DeltaTime);


	// Slowly regenerate shields.

	const float Strength = GetShieldStrength();

	if(Strength < 1.0f)
	{
		SetShieldStrength(FMath::Min(1.0f, Strength + DeltaTime / 20));
	}


	if(Sprite)
	{
		Sprite->SetTint(MakeBlendedColor(C_RED, C_WHITE, Strength)); // Make the ship appear redder as its shields weaken.
		Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);
	}
	

	// Reduce the double guns if they're active.

	if(AreDoubleGunsActive())
	{
		auto OldAmt = DoubleGunsLeft.Get();

		const auto NewAmt = FMath::Max(0.0f, OldAmt - DeltaTime * PLAYER_DOUBLEGUNS_LOSS);

		DoubleGunsLeft.Set(NewAmt);

		if(NewAmt == 0.0f && OldAmt > 0.0f) // could also say if(NewAmt == Amt), if PLAYER_DOUBLEGUNS_LOSS is always nonzero.
		{
			GMessageMediator.TellUser(TEXT("DUAL LASER CANNON ENERGY DEPLETED"), MESSAGE_DURATION_IMPORTANT, EDisplayMessage::DualCannonsLevelChanged);
		}
	}

	// Reduce the invincibility if it's active.

	if(IsInvincibilityActive())
	{
		auto OldAmt = InvincibilityLeft.Get();

		const auto NewAmt = FMath::Max(0.0f, OldAmt - DeltaTime * PLAYER_INVINCIBILITY_LOSS);

		InvincibilityLeft.Set(NewAmt);

		if(NewAmt == 0.0f && OldAmt > 0.0f) // could also say if(NewAmt == Amt), if PLAYER_INVINCIBILITY_LOSS is always nonzero.
		{
			GMessageMediator.TellUser(TEXT("INVINCIBILITY DEPLETED"), MESSAGE_DURATION_IMPORTANT, EDisplayMessage::InvincibilityLevelChanged);

			if(Sprite)
			{
				Sprite->SetAtlas(GGameObjectResources.Get(EObjType::PLAYER).Atlas->Atlas);
			}
		}
	}
}


void Defcon::CPlayerShip::DrawSmall(FPainter& Painter, const I2DCoordMapper& Mapper, FSlateBrush&) const
{
	// Draw a white 5 x 5 px diamond.
	CFPoint P;

	Mapper.To(Position, P);

	const auto S = FVector2D(12, 12);// RadarBrush.GetImageSize();

	const FSlateLayoutTransform Translation(FVector2D(P.x, P.y) - S / 2);

	const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*Painter.OutDrawElements,
		Painter.LayerId,
		Geometry.ToPaintGeometry(),
		&RadarBrush, // todo: can use provided brush
		ESlateDrawEffect::None,
		RadarBrush.TintColor.GetSpecifiedColor() * Painter.RenderOpacity);
}


void Defcon::CPlayerShip::FireLaserWeapon(CGameObjectCollection& goc)
{
	LaserWeapon.Fire(goc);

	if(AreDoubleGunsAvailable())
	{
		SecondaryLaserWeapon.Fire(goc);
	}
}


void Defcon::CPlayerShip::ImpartForces(float DeltaTime)
{
	if(!bCanMove)
	{
		return;
	}

	if(!CONTROLLER_EXPLICIT_REVERSE)
	{
		Super::ImpartForces(DeltaTime);
		return;
	}

	// Let parent class handle horizontal forces.
	ThrustVector.y = 0.0f;
	Super::ImpartForces(DeltaTime);

	// Now handle vertical force in our way.
	const float kVertMotionPxPerSec = 300.0f;

	if(NavControls[ENavControl::Up].bActive)
	{
		const float TimeHeld = NavControlDuration(ENavControl::Up);
		Position.MulAdd({ 0.0f, FMath::Min((kVertMotionPxPerSec * TimeHeld * 2) + kVertMotionPxPerSec/2, kVertMotionPxPerSec) }, DeltaTime);
	}
	else if(NavControls[ENavControl::Down].bActive)
	{
		const float TimeHeld = NavControlDuration(ENavControl::Down);
		Position.MulAdd({ 0.0f, -FMath::Min((kVertMotionPxPerSec * TimeHeld * 2) + kVertMotionPxPerSec/2, kVertMotionPxPerSec) }, DeltaTime);
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------

Defcon::CDestroyedPlayerShip::CDestroyedPlayerShip()
{
	Type          = Defcon::EObjType::DESTROYED_PLAYER;
	CreatorType   = Defcon::EObjType::PLAYER;
	Lifespan      = DESTROYED_PLAYER_LIFETIME;
	bMortal       = true;
	bCanBeInjured = false;
	Age           = 0.0f;

	CreateSprite(Defcon::EObjType::DESTROYED_PLAYER);
}


void Defcon::CDestroyedPlayerShip::InitDestroyedPlayer(const CFPoint& ShipP, const CFPoint& ShipS, float ParticleSpeed, float MinParticleLifetime, float MaxParticleLifetime)
{
	for(auto& ParticleGroup : ParticleGroups)
	{
		ParticleGroup.Init(ShipP, ShipS, ParticleSpeed, MinParticleLifetime, MaxParticleLifetime);
	}
}


void Defcon::CDestroyedPlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Age > DESTROYED_PLAYER_FLASH_DURATION)
	{
		// After the initial red-white flicker, stop showing the ship
		//Sprite->Hide();
		// Uninstall and reset the sprite; this will cause paint events to call Draw method.
		if(Sprite)
		{
			UninstallSprite();
			Sprite.Reset();

			GMessageMediator.PlaySound(Defcon::EAudioTrack::Ship_exploding2b);
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


void Defcon::CDestroyedPlayerShip::Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)
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


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
