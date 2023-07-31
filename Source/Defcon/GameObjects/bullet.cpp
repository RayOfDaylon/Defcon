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
	Type = EObjType::BULLET;
	bInjurious = true;
	bCanBeInjured = false;

	RadarColor.A = 0.0f;
	m_color = MakeColorFromComponents(255, 255, 255);
	bMortal = true;
	m_fOrgLifespan = Lifespan = FRANDRANGE(MIN_BULLETAGE, MAX_BULLETAGE);
	m_fRadius = 5;
	m_fSpeed = FRAND * 300 + 352;
}


void Defcon::IBullet::Move(float fTime)
{
	// Make the bullet flicker.
	if(Sprite)
	{
		Sprite->SetCurrentCel(IRAND(NumSpriteCels));
	}

	// Self-destruct if we pass out of the vertical bounds.
	if(Position.y < 0 || Position.y > gpArena->GetHeight())
	{
		this->MarkAsDead();
	}

	// todo: If the object processor is wrapping x-pos, why are we wrapping it here?
	// A baiter managed to emit a bullet that had a -xpos, I guess if it was near the origin
	// it could do that in a single frame, so if we use xpos here we have to wrap it.
	if(BULLETS_HIT_TERRAIN)
	{
		WRAP(Position.x, 0, gpArena->GetWidth());
		check(Position.x >= 0.0f && Position.x < gpArena->GetWidth());
		if(Position.y <= gpArena->GetTerrainElev(Position.x))
		{
			this->MarkAsDead();
		}
	}

	Position.MulAdd(Orientation.Fwd, fTime * m_fSpeed);
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

	NumSpriteCels = GameObjectResources.Get(Type).Atlas->Atlas.NumCels;
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

	NumSpriteCels = GameObjectResources.Get(Type).Atlas->Atlas.NumCels;
}


#ifdef _DEBUG
const char* Defcon::CBullet::GetClassname() const
{
	static char* psz = "ThinBullet";
	return psz;
}
#endif

