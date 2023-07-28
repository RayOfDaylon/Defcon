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
	m_parentType = m_type;
	m_type = ObjType::SPACEHUM;
	m_pointValue = SPACEHUM_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_LIGHT;

	m_fAnimSpeed = FRAND * 0.05f + 0.15f;
	m_bCanBeInjured = true;
	m_bIsCollisionInjurious = true;
	m_fBrightness = FRANDRANGE(0.9f, 1.0f);

	m_fSpeed = (float)gDefconGameInstance->GetScore() / 250;
	m_fSpeed *= FRANDRANGE(0.9f, 1.33f);

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
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
	m_inertia = m_pos;

	CPlayer* pTarget = &gpArena->GetPlayerShip();

	if(pTarget->IsAlive())
	{
		gpArena->Direction(m_pos, pTarget->m_pos, m_orient.fwd);
		CFPoint budge((float)sin(FRAND * PI * 2), (float)cos(FRAND * PI * 2));

		if(m_fAge < 2.0f)
		{
			budge.x *= 0.1f;
			m_orient.fwd.x *= 0.1f;
		}

		budge *= SFRAND * 200.0f * fTime;
		m_pos.muladd(m_orient.fwd, fTime * m_fSpeed);
		m_pos += budge;
	}

	m_inertia = m_pos - m_inertia;
}


void Defcon::CSpacehum::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CSpacehum::Explode(CGameObjectCollection& debris)
{
	const int cby = CGameColors::gray;

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	for(size_t i = 0; i < 10; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->m_pos = m_pos;
		pFlak->m_orient = m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}
}
