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
	bPreferTargetUnderside = FRAND >= 0.5f;
	FireWeaponCountdown    = FRANDRANGE(0.75f, 2.0f);

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
}


Defcon::CBaiter::~CBaiter()
{
}


#ifdef _DEBUG
const char* Defcon::CBaiter::GetClassname() const
{
	static char* psz = "Baiter";
	return psz;
};
#endif


void Defcon::CBaiter::Move(float DeltaTime)
{
	// Baiters chase the player at high speed and fire at them.

	if(TargetPtr == nullptr)
	{
		return;
	}
	
	const float xd = gpArena->HorzDistance(Position, TargetPtr->Position);
	float d = Drag;
	float m = Mass;
	// Increase drag as baiter near player.
	float t = NORM_(xd, 0.0f, 500.0f);
	t = FMath::Min(1.0f, t);
	//Drag = LERP(Drag * 30.0f, Drag, t);
	Mass = LERP(Mass * 3.0f, Mass, t);

	CEnemy::Move(DeltaTime);

	Drag = d;
	Mass = m;


	Inertia = Position;

	// Don't move until after materialization is complete.
	//if(Age > ENEMY_BIRTHDURATION)
	{
		// Thrust in a way that will take us towards 
		// the player. Only thrust vertically if we 
		// are not within range.

		CFPoint delta, target;
		target = TargetPtr->Position;
		const float vsign = bPreferTargetUnderside ? -1.0f : 1.0f;

		target += CFPoint(
			(float)cos(Age) * 300.0f * SGN(TargetPtr->Orientation.Fwd.x),
			vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

		const float Xd = gpArena->Direction(Position, target, delta);
		bool bMoveTowards = (Xd > 150 || SGN(Orientation.Fwd.x) != SGN(TargetPtr->Orientation.Fwd.x));

		if(bMoveTowards)
		{
			const int32 ctl = (delta.x < 0)	? ILiveGameObject::ctlBack : ILiveGameObject::ctlFwd;
			const int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack) ? ILiveGameObject::ctlFwd : ILiveGameObject::ctlBack;

			if(this->NavControl_Duration(ctl) == 0)
			{
				ControlStartTime[ctl] = GameTime();
			}

			SetNavControl(ctl, true, ControlStartTime[ctl]);
			SetNavControl(ctl2, false, 0);
		}
		else
		{
			// Parallel the player.
			SetNavControl(ILiveGameObject::ctlBack, false, 0);
			SetNavControl(ILiveGameObject::ctlFwd, false, 0);
		}

		// Vertical.
		bMoveTowards = (Xd > 50);

		if(bMoveTowards)
		{
			const int32 ctl  = (delta.y < 0) ? ILiveGameObject::ctlDown : ILiveGameObject::ctlUp;
			const int32 ctl2 = (ctl ==  ILiveGameObject::ctlDown) ? ILiveGameObject::ctlUp : ILiveGameObject::ctlDown;

			if(NavControl_Duration(ctl) == 0)
			{
				ControlStartTime[ctl] = GameTime();
			}

			SetNavControl(ctl, true, ControlStartTime[ctl]);
			SetNavControl(ctl2, false, 0);
		}
		else
		{
			SetNavControl(ILiveGameObject::ctlDown, false, 0);
			SetNavControl(ILiveGameObject::ctlUp, false, 0);
		}

		if(gpArena->IsPointVisible(Position))
		{
			FireWeaponCountdown -= DeltaTime;

			if(FireWeaponCountdown <= 0.0f)
			{
				FireWeaponCountdown = FRANDRANGE(0.75f, 2.0f);
				gpArena->FireBullet(*this, Position, 1, 1);
			}
		}		
	}

	Inertia = Position - Inertia;
}


Defcon::EColor Defcon::CBaiter::GetExplosionColorBase() const
{
	return EColor::yellow;
}
