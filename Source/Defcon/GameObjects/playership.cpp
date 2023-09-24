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



#define DEBUG_MODULE      0

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
}


void Defcon::CPlayerShip::InitPlayerShip()
{
	Drag      = PLAYER_DRAG;
	MaxThrust = PLAYER_MAXTHRUST;
	Mass      = PLAYER_MASS;

	DeactivateDoubleGuns();
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

	if(Sprite)
	{
		Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);
	}

	// Slowly regenerate shields.

	const float Strength = GetShieldStrength();

	if(Strength < 1.0f)
	{
		SetShieldStrength(FMath::Min(1.0f, Strength + DeltaTime / 20));
	}

	// Make the ship appear redder as its shields weaken.
	Sprite->SetTint(MakeBlendedColor(C_RED, C_WHITE, Strength));
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

	if(AreDoubleGunsActive())
	{
		SecondaryLaserWeapon.Fire(goc);
	}

#if 0
	//if(Velocity.y != 0 && FRAND <= LASER_MULTI_PROB)
	{
		// Fire extra bolts in the vthrust dir.
		for(int32 i = 0; i < 1/*LASER_EXTRA_COUNT*/; i++)
		{
			//CFPoint Offset(0.0f, Velocity.y > 0 ? 1.0f : -1.0f);

			// Use the second mount point.
			//const CFPoint Offset(Orientation.Fwd.x * -10.0f, 5.0f);

			/*if(BRAND)
			{
				Offset *= 2;
			}*/

			CFPoint Backup(Position);
			Position += Offset;
			LaserWeapon.Fire(goc);
			Position = Backup;
		}
	}
#endif
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


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
