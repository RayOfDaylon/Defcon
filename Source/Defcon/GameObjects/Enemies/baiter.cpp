// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	baiter.cpp
	Baiter type for Defcon game.

	The baiter is an enemy ship that normally 
	appears late in a wave, flies very fast 
	(usually faster than the player) and tries 
	to shoot the player.
*/


#include "baiter.h"

#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/obj_types.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconLogging.h"



Defcon::CBaiter::CBaiter()
{
	ParentType             = Type;
	Type                   = EObjType::BAITER;

	PointValue             = BAITER_VALUE;
	RadarColor             = MakeColorFromComponents(192, 192, 0);
	AnimSpeed              = FRAND * 0.35f + 0.15f;

	Drag                   = PLAYER_DRAG * 0.15f;
	MaxThrust              = PLAYER_MAXTHRUST * 9.0f;
	Mass                   = PLAYER_MASS * 20;
	bIsCollisionInjurious  = true;
	bPreferTargetUnderside = BRAND;
	FireWeaponCountdown    = FRANDRANGE(0.75f, 2.0f);

	CreateSprite(Type);
	BboxRadius = GGameObjectResources.Get(Type).Size;
}


Defcon::CBaiter::~CBaiter()
{
}


void Defcon::CBaiter::Tick(float DeltaTime)
{
	// Baiters chase the player at high speed and fire at them.

	if(TargetPtr == nullptr)
	{
		return;
	}
	
	const float xd = GArena->HorzDistance(Position, TargetPtr->Position);
	float d = Drag;
	float m = Mass;
	// Increase drag as baiter near player.
	float t = NORM_(xd, 0.0f, 500.0f);
	t = FMath::Min(1.0f, t);
	//Drag = LERP(Drag * 30.0f, Drag, t);
	Mass = LERP(Mass * 3.0f, Mass, t);

	CEnemy::Tick(DeltaTime);

	Drag = d;
	Mass = m;


	Inertia = Position;

	
	{
		// Thrust in a way that will take us towards the player. 
		// Only thrust vertically if we are not within range.

		CFPoint delta, target;
		target = TargetPtr->Position;
		const float vsign = bPreferTargetUnderside ? -1.0f : 1.0f;

		target += CFPoint(
			cosf(Age) * 300.0f * SGN(TargetPtr->Orientation.Fwd.x),
			vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

		const float Xd = GArena->ShortestDirection(Position, target, delta);
		bool bMoveTowards = (Xd > 150 || SGN(Orientation.Fwd.x) != SGN(TargetPtr->Orientation.Fwd.x));

		if(bMoveTowards)
		{
			const int32 ctl = (delta.x < 0)	? ENavControl::Back : ENavControl::Fwd;
			const int32 ctl2 = (ctl ==  ENavControl::Back) ? ENavControl::Fwd : ENavControl::Back;

			SetNavControl(ctl, true, NavControlDuration(ctl));
			SetNavControl(ctl2, false, 0);
		}
		else
		{
			// Parallel the player.
			SetNavControl(ENavControl::Back, false, 0);
			SetNavControl(ENavControl::Fwd,  false, 0);
		}

		// Vertical.
		bMoveTowards = (Xd > 50);

		if(bMoveTowards)
		{
			const int32 ctl  = (delta.y < 0) ? ENavControl::Down : ENavControl::Up;
			const int32 ctl2 = (ctl ==  ENavControl::Down) ? ENavControl::Up : ENavControl::Down;

			SetNavControl(ctl, true, NavControlDuration(ctl));
			SetNavControl(ctl2, false, 0);
		}
		else
		{
			SetNavControl(ENavControl::Down, false, 0);
			SetNavControl(ENavControl::Up, false, 0);
		}

		if(IsOurPositionVisible())
		{
			FireWeaponCountdown -= DeltaTime;

			if(FireWeaponCountdown <= 0.0f)
			{
				FireWeaponCountdown = FRANDRANGE(0.75f, 2.0f);
				GArena->FireBullet(*this, Position, 1, 1);
			}
		}		
	}

	Inertia = Position - Inertia;
}


Defcon::EColor Defcon::CBaiter::GetExplosionColorBase() const
{
	return EColor::Yellow;
}
