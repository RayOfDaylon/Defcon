// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	MilitaryMission.cpp
	Base class of military missions.
*/


#include "MilitaryMission.h"

#include "DefconUtils.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/player.h"
#include "GameObjects/human.h"
#include "GameObjects/Auxiliary/stargate.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



void Defcon::CMilitaryMission::Init(UDefconPlayViewBase* p)
{
	IMission::Init(p);

	// Military missions have stargates.
	this->AddStargate();

#if 0
	// Turrets were an experiment, maybe later we'll have them.
	// Add some turrets.

	check(m_pArena != nullptr);

	if(m_pArena->HasTerrain())
	{
		for(int i = 0; i < 10; i++)
		{
			//auto pEvt = new CCreateEnemyEvent;
			pEvt->Init(p);
			pEvt->m_objtype = ObjType::TURRET;
			pEvt->m_when = GameTime();
			float wp = m_pArena->GetWidth();
			float x = FRAND * wp;
			x = (float)fmod(x, wp);
			float y = p->GetTerrainElev(x) - 10;
			pEvt->m_where.set(x, y);
			pEvt->m_bMissionTarget = false; // can leave turrets alive w/o aborting mission
			pEvt->m_bMaterializes = false;
			this->AddEvent(pEvt);
		}
	}
#endif
}


void Defcon::CMilitaryMission::AddStargate()
{
	check(m_pArena != nullptr);
	m_pStargate = new CStargate;
	CFPoint pos(0.5f, 0.75f);
	pos.mul(CFPoint(m_pArena->GetWidth(), m_pArena->GetHeight()));
	m_pStargate->m_pos = pos; 
	m_pStargate->InstallSprite();
	m_pArena->GetObjects().Add(m_pStargate);

	// Since the stargate is stationary, cache its rectangle.
	StargateRect.set(m_pStargate->m_pos);
	StargateRect.inflate(m_pStargate->m_bboxrad);
}


bool Defcon::CMilitaryMission::PlayerInStargate() const
{
	return StargateRect.ptinside(gpArena->GetPlayerShip().m_pos);
}


bool Defcon::CMilitaryMission::Update(float DeltaTime)
{
	check(m_pArena == gpArena);

	if(!IMission::Update(DeltaTime))
	{
		return false;
	}

	m_fRepopCounter += DeltaTime;

	if(this->PlayerInStargate())
	{
		m_pArena->AllStopPlayerShip();

		if(IsCompleted())
		{
			// Warp to next mission.
			auto& Player = m_pArena->GetPlayerShip();
			Player.EnableInput(false);

			// If the player is carrying any humans, redistribute them back to the ground to avoid clumping in next mission.
			gDefconGameInstance->GetHumans().ForEach([this](IGameObject* pObj)
			{
				auto Human = static_cast<CHuman*>(pObj);

				if(Human->IsBeingCarried() && Human->GetCarrier()->GetType() == ObjType::PLAYER)
				{
					Human->SetToNotCarried();
					Human->m_pos.set(FRANDRANGE(0.0f, m_pArena->GetWidth() - 1), 0.04f * m_pArena->GetHeight()); 
				}
			});
 			return false;
		}

		m_pArena->TransportPlayerShip();
	}

	if(this->HostilesRemaining() > 0)
	{
		this->MakeTargets(DeltaTime, m_pArena->GetPlayerShip().m_pos);
	}

	return true;
}


int32 Defcon::CMilitaryMission::HostilesRemaining() const
{
	return m_nHostilesRemaining;
}


int32 Defcon::CMilitaryMission::HostilesInPlay() const
{
	return (int32)m_pArena->GetEnemies().Count();
}


void Defcon::CMilitaryMission::AddMissionCleaner(const CFPoint& where)
{
	// When a mission drags on too long, it's time 
	// to throw in various nonmission hostiles at an increasing 
	// frequency until the player wipes out the critical opponents.

	const ObjType MunchieTypes[] = { ObjType::PHRED, ObjType::BIGRED, ObjType::MUNCHIE };

	if(m_fAge - m_fLastCleaner >= m_CleanerFreq)
	{
		if(m_CleanerFreq > 0.5f)
		{
			m_CleanerFreq -= 0.5f;
		}

		this->AddNonMissionTarget(FRAND < BAITER_PROB ? ObjType::BAITER : MunchieTypes[IRAND(3)], where);
	}
}


void Defcon::CMilitaryMission::AddNonMissionTarget(ObjType objType, const CFPoint& where)
{
	check(m_pArena == gpArena);

	m_fLastCleaner = m_fAge;
	//CCreateEnemyEvent* p = new CCreateEnemyEvent;
	//p->Init(m_pArena);
	//p->m_objtype = objType;
	//p->m_when = GameTime();
	//p->m_bMaterializes = true;
	//p->m_bMissionTarget = false;
	float wp = m_pArena->GetWidth();
	float x = (FRAND - 0.5f) * 1.0f * m_pArena->GetDisplayWidth() + where.x;
	x = (float)fmod(x, wp);
	float y = FRANDRANGE(0.3f, 0.8f) * m_pArena->GetHeight();
	//p->m_where.set(x, y);
	//this->AddEvent(p);

	m_pArena->CreateEnemy(objType, CFPoint(x, y), 0.0f, true, false);
}


void Defcon::CMilitaryMission::AddBaiter(const CFPoint& where)
{
	m_fLastCleaner = m_fAge;
	//CCreateEnemyEvent* p = new CCreateEnemyEvent;
	//p->Init(m_pArena);
	//p->m_objtype = ObjType::BAITER;
	//p->m_when = GameTime();
	//p->m_bMaterializes = true;
	//p->m_bMissionTarget = false;
	float wp = m_pArena->GetWidth();
	float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
	x = (float)fmod(x, wp);
	float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();
	//p->m_where.set(x, y);
	//this->AddEvent(p);

	m_pArena->CreateEnemy(ObjType::BAITER, CFPoint(x, y), 0.0f, true, false);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
