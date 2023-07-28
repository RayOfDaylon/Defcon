// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission.cpp
	IMission base class
	Defines commonalities of missions in Defence Condition.
*/


#include "mission.h"

#include "DefconUtils.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/player.h"
#include "GameObjects/human.h"
#include "GameObjects/Auxiliary/stargate.h"
#include "GameObjects/Enemies/lander.h"
#include "GameObjects/Enemies/guppy.h"
#include "GameObjects/Enemies/hunter.h"
#include "GameObjects/Enemies/bomber.h"
#include "GameObjects/Enemies/pod.h"
#include "GameObjects/Enemies/swarmer.h"
#include "GameObjects/Enemies/baiter.h"
#include "GameObjects/Enemies/firebomber.h"
#include "GameObjects/Enemies/fireball.h"
#include "GameObjects/Enemies/dynamo.h"
#include "GameObjects/Enemies/spacehum.h"
#include "GameObjects/Enemies/reformer.h"
#include "GameObjects/Enemies/bouncer.h"
#include "GameObjects/Enemies/munchies.h"
#include "GameObjects/Enemies/ghost.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




void Defcon::IMission::Init(UDefconPlayViewBase* p)
{
	m_pArena = p; 

	this->CreateTerrain();
	this->AddHumanoids();

	m_pArena->GetPlayerShip().EnableInput(false);
}


bool Defcon::IMission::Update(float f) 
{
	m_fAge += f; 
	m_events.Process();
	return true; 
}


void Defcon::IMission::CreateTerrain()
{
	m_pArena->CreateTerrain();
}


void Defcon::IMission::AddHumanoids()
{
	// Most missions have humanoids. 
	// We mod the game's humanoids for the arena.

	auto& Humans = m_pArena->GetHumans();

	Humans.ForEach([&](Defcon::IGameObject* Obj)
	{
		auto Human = static_cast<CHuman*>(Obj);
		Human->m_pObjects  = &m_pArena->GetObjects();
		Human->m_pObjects2 = &m_pArena->GetEnemies();

		// If we're on wave 2 or higher, don't move the humans
		// except to reset them vertically.
		Human->InitHuman(CFPoint(
			(gDefconGameInstance->GetScore() == 0) ? FRAND * m_pArena->GetWidth() * HUMAN_DISTRIBUTION : Human->m_pos.x, 
			FRAND * 5 + 25));
	}
	);
}


void Defcon::IMission::Conclude()
{
}

// --------------------------------------------------------

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
}


bool Defcon::CMilitaryMission::PlayerInStargate() const
{
	check(m_pArena == gpArena);

	// See if the player entered the stargate, and if so, 
	// conclude the mission.
	// todo: we want arena_play to conclude, and 
	// chain to arena_stargate. The player ship should 
	// also stop moving and responding to controls.
	auto& Player = m_pArena->GetPlayerShip();

	
	CFRect	rGate(m_pStargate->m_pos);//, rPlayer(Player.m_pos);
	rGate.inflate(m_pStargate->m_bboxrad);
	//rPlayer.inflate(Player.m_bboxrad);
	return rGate.ptinside(Player.m_pos);
	

	return false;
}


bool Defcon::CMilitaryMission::Update(float fElapsed)
{
	check(m_pArena == gpArena);

	if(IMission::Update(fElapsed))
	{
		m_fRepopCounter += fElapsed;

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

	// todo: #if 0 this out when stargate working.
		if(this->HostilesRemaining() > 0)
		{
			CFPoint where;
			//if(m_pArena->GetPlayer() != nullptr)
				where = m_pArena->GetPlayerShip().m_pos;
			this->MakeTargets(fElapsed, where);
			// todo: should we be calling MakeTargets
			// when player is null?
			//return true;
		}
#if 0
		return (this->HostilesInPlay() > 0);
#endif
		return true;
	}
	return false;



#if 0
	if(IMission::Update(fElapsed))
	{
		m_fRepopCounter += fElapsed;

		if(this->HostilesRemaining() > 0)
		{
			CFPoint where;
			if(gpArena->GetPlayerShip() != nullptr)
				where = gpArena->GetPlayerShip()->m_pos;
			this->MakeTargets(fElapsed, where);
			return true;
		}
		return (this->HostilesInPlay() > 0);
	}
	return false;
#endif
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
