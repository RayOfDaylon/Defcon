// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	spacehum.cpp
	Space Hum enemy type for Defcon game.
*/


#include "spacehum.h"

#include "DefconUtils.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CSpacehum::CSpacehum()
{
	ParentType = Type;
	Type = EObjType::SPACEHUM;
	PointValue = SPACEHUM_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_LIGHT;

	AnimSpeed = FRAND * 0.05f + 0.15f;
	bCanBeInjured = true;
	bIsCollisionInjurious = true;
	m_fBrightness = FRANDRANGE(0.9f, 1.0f);

	m_fSpeed = (float)gDefconGameInstance->GetScore() / 250;
	m_fSpeed *= FRANDRANGE(0.9f, 1.33f);

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


#ifdef _DEBUG
const char* Defcon::CSpacehum::GetClassname() const
{
	static char* psz = "Spacehum";
	return psz;
};
#endif


void Defcon::CSpacehum::Move(float fTime)
{
	// Just float around drifting horizontally.

	CEnemy::Move(fTime);
	Inertia = Position;

	CPlayer* pTarget = &gpArena->GetPlayerShip();

	if(pTarget->IsAlive())
	{
		gpArena->Direction(Position, pTarget->Position, Orientation.Fwd);
		CFPoint budge((float)sin(FRAND * PI * 2), (float)cos(FRAND * PI * 2));

		if(Age < 2.0f)
		{
			budge.x *= 0.1f;
			Orientation.Fwd.x *= 0.1f;
		}

		budge *= SFRAND * 200.0f * fTime;
		Position.MulAdd(Orientation.Fwd, fTime * m_fSpeed);
		Position += budge;
	}

	Inertia = Position - Inertia;
}


void Defcon::CSpacehum::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CSpacehum::Explode(CGameObjectCollection& debris)
{
	const int cby = CGameColors::gray;

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 10; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->Position = Position;
		pFlak->Orientation = Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.Fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->Orientation.Fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}
}
