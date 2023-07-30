// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	dynamo.cpp
	Dynamo enemy type for Defcon game.
*/


#include "dynamo.h"

#include "DefconUtils.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CDynamo::CDynamo()
{
	m_parentType = m_type;
	m_type = ObjType::DYNAMO;
	m_pointValue = DYNAMO_VALUE;
	m_orient.fwd.Set(1.0f, 0.0f);
	m_smallColor = C_LIGHT;
	m_fAnimSpeed = FRAND * 0.05f + 0.15f;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CDynamo::~CDynamo()
{
}

#ifdef _DEBUG
const char* Defcon::CDynamo::GetClassname() const
{
	static char* psz = "Dynamo";
	return psz;
};
#endif


void Defcon::CDynamo::Move(float fTime)
{
	// Just float around drifting horizontally.

	CEnemy::Move(fTime);
	m_inertia = m_pos;


	m_SpawnSpacehumCountdown -= fTime;

	if(m_SpawnSpacehumCountdown <= 0.0f 
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(m_pos))
	{
		gpArena->CreateEnemy(ObjType::SPACEHUM, m_pos, 0.0f, false, false);

		const auto XP = gDefconGameInstance->GetScore();
		float T = NORM_(XP, 0.0f, 50000.0f);
		T = CLAMP(T, 0.0f, 1.0f);

		m_SpawnSpacehumCountdown = LERP(2.5f, 0.5f, T);
		m_SpawnSpacehumCountdown += Daylon::FRandRange(0.0f, 0.25f);

		// todo: maybe have dynamo take a break after spitting out lots of hums, then resume.
	}


	if(m_fAge < 1.0f)
		m_orgY = m_pos.y;

	m_pos.x += (float)sin(5.0f * m_fAge) * 1.0f;
	float sn = (float)sin(1.0f * m_fAge) * 1.0f;
	m_pos.y = m_orgY + sn;

	m_pos.y = CLAMP(m_pos.y, 0, gpArena->GetHeight()-1);

	m_inertia = m_pos - m_inertia;
}


void Defcon::CDynamo::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}
