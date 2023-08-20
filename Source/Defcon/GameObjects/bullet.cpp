// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	bullet.cpp
	Bullet weaponsfire routines for Defcon game.
*/


#include "bullet.h"

#include "Globals/prefs.h"
#include "Arenas/DefconPlayViewBase.h"


#define MIN_BULLETAGE	2.5f
#define MAX_BULLETAGE	3.5f


Defcon::IBullet::IBullet()
{
	ParentType = Type;
	Type       = EObjType::BULLET;

	bInjurious    = true;
	bCanBeInjured = false;
	Color         = C_WHITE;
	bMortal       = true;
	Lifespan      = FRANDRANGE(MIN_BULLETAGE, MAX_BULLETAGE);
	Speed         = FRAND * 300 + 352;
}


void Defcon::IBullet::Move(float DeltaTime)
{
	// Make the bullet flicker.
	if(Sprite)
	{
		Sprite->SetCurrentCel(IRAND(NumSpriteCels));
	}

	// Self-destruct if we pass out of the vertical bounds.
	if(Position.y < 0 || Position.y > GArena->GetHeight())
	{
		MarkAsDead();
	}

	// todo: If the object processor is wrapping x-pos, why are we wrapping it here?
	// A baiter managed to emit a bullet that had a -xpos, I guess if it was near the origin
	// it could do that in a single frame, so if we use xpos here we have to wrap it.
	if(BULLETS_HIT_TERRAIN)
	{
		WRAP(Position.x, 0, GArena->GetWidth());
		check(Position.x >= 0.0f && Position.x < GArena->GetWidth());

		if(Position.y <= GArena->GetTerrainElev(Position.x))
		{
			MarkAsDead();
		}
	}

	Position.MulAdd(Orientation.Fwd, DeltaTime * Speed);
}


void Defcon::IBullet::GetInjurePt(CFPoint& pt) const
{
	pt = Position;
}


bool Defcon::IBullet::TestInjury(const CFRect& r) const
{
	return r.PtInside(Position);
}

// ------------------------------------------------------------------

Defcon::CBullet::CBullet()
{
	ParentType = Type;
	Type = EObjType::BULLET_ROUND;

	CreateSprite(Type);
	Sprite->IsStatic = true;

	NumSpriteCels = GGameObjectResources.Get(Type).Atlas->Atlas.NumCels;
}


#ifdef _DEBUG
const char* Defcon::CBullet::GetClassname() const
{
	static char* psz = "Bullet";
	return psz;
}
#endif


// -------------------------------------------------------------------------

Defcon::CThinBullet::CThinBullet()
{
	ParentType = Type;
	Type = EObjType::BULLET_THIN;

	CreateSprite(Type);
	Sprite->IsStatic = true;

	NumSpriteCels = GGameObjectResources.Get(Type).Atlas->Atlas.NumCels;
}


#ifdef _DEBUG
const char* Defcon::CBullet::GetClassname() const
{
	static char* psz = "ThinBullet";
	return psz;
}
#endif

