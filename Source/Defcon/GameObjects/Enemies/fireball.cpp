// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	fireball.cpp
	Fireball entity for Defcon game.

	The fireball is shot (normally) by firebombers.
	It is a bit larger than a swarmer and looks 
	like a fluctuating orange/reddish hairball.
	Unlike a bullet, it can be shot down by the player.
	The movement routine pretends gravity exists, 
	so if we reach the bottom of the screen, 
	we're done.

	In Stargate, firebombers only shot fireballs.
*/

#include "fireball.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CFireball::CFireball()
{
	ParentType = Type;
	Type       = EObjType::FIREBALL;

	PointValue            = FIREBALL_VALUE;
	bCanBeInjured         = true;
	bIsCollisionInjurious = true;
	RadarColor            = C_ORANGE;
	Orientation.Fwd.y     = -1.0f;
	
	// the shooter sets the speed.
	// Depending on accuracy, the shooter takes gravity 
	// into account so that the fireball will hit the 
	// target. Again, the shot can be at the target's 
	// current position or be lead.

	CreateSprite(Type);

	BboxRadius = GGameObjectResources.Get(Type).Size / 2;
}


#ifdef _DEBUG
const char* Defcon::CFireball::GetClassname() const
{
	static char* psz = "Fireball";
	return psz;
}
#endif



void Defcon::CFireball::Move(float DeltaTime)
{
	if(Age == 0.0f)
	{
		// Do first-time stuff.

		if(TargetPtr == nullptr)
		{
			MarkAsDead();
			return;
		}
		else
		{
			CFPoint dir;
			Speed = (FRAND * 0.5f + 1.0f) * GArena->ShortestDirection(Position, TargetPtr->Position, dir);
			Orientation.Fwd.x = (float)(SGN(dir.x));
			Orientation.Fwd.y = (float)(SGN(dir.y));
		}
	}

	CEnemy::Move(DeltaTime);

	Inertia = Position;

	CFPoint Motion(Orientation.Fwd);

	Motion.x *= Speed * DeltaTime;
	Motion.y -= Age * Age;

	Position += Motion;

	Inertia = Position - Inertia;

	// Die if go below the arena floor.
	if(Position.y < 0)
	{
		MarkAsDead();
		return;
	}

	// Explode if we hit the ground.
	if(BULLETS_HIT_TERRAIN)
	{
		WRAP(Position.x, 0, GArena->GetWidth());

		if(Position.y <= GArena->GetTerrainElev(Position.x))
		{
			if(FIREBALLS_EXPLODE_ON_GROUND)
			{
				GArena->DestroyObject(this);	
			}
			else
			{
				MarkAsDead();
			}
		}
	}
}


Defcon::EColor Defcon::CFireball::GetExplosionColorBase() const
{
	return EColor::Red;
}


float Defcon::CFireball::GetExplosionMass() const
{
	return 0.33f;
}


void Defcon::CFireball::Explode(CGameObjectCollection& Debris)
{
	FExplosionParams Params;

	Params.NumParticles = 20;
	Params.YoungColor[0] = 
	Params.YoungColor[1] = BRAND ? EColor::Yellow : EColor::Orange;
	Params.OldColor[0] = EColor::Yellow;
	Params.OldColor[1] = EColor::Red;
	Params.MaxParticleSize = 5;

	AddExplosionDebris(Params, Debris);
}
