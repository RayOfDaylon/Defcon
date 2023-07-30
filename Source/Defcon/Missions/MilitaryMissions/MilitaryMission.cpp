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
	// Turrets are an experiment, maybe later we'll have them.
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
			pEvt->m_where.Set(x, y);
			pEvt->m_bMissionTarget = false; // can leave turrets alive w/o aborting mission
			pEvt->m_bMaterializes = false;
			this->AddEvent(pEvt);
		}
	}
#endif
}


void Defcon::CMilitaryMission::AddEnemySpawnInfo(const FEnemySpawnCounts& EnemySpawnCounts)
{
	EnemySpawnCountsArray.Add(EnemySpawnCounts);

	int32 WaveNumber = 1;

	for(auto EnemyTypeCount : EnemySpawnCounts.NumPerWave)
	{
		m_nHostilesRemaining += EnemyTypeCount;

		if(EnemySpawnCounts.Kind == ObjType::LANDER)
		{
			m_nLandersRemaining	+= EnemyTypeCount;
		}

		if(EnemyTypeCount > 0 && WaveNumber > MaxWaves)
		{
			MaxWaves = WaveNumber;
		}

		WaveNumber++;
	}
}


void Defcon::CMilitaryMission::AddStargate()
{
	check(m_pArena != nullptr);
	m_pStargate = new CStargate;
	CFPoint pos(0.5f, 0.75f);
	pos.Mul(CFPoint(m_pArena->GetWidth(), m_pArena->GetHeight()));
	m_pStargate->m_pos = pos; 
	m_pStargate->InstallSprite();
	m_pArena->GetObjects().Add(m_pStargate);

	// Since the stargate is stationary, cache its rectangle.
	StargateRect.Set(m_pStargate->m_pos);
	StargateRect.Inflate(m_pStargate->m_bboxrad);
}


bool Defcon::CMilitaryMission::PlayerInStargate() const
{
	return StargateRect.PtInside(gpArena->GetPlayerShip().m_pos);
}


void Defcon::CMilitaryMission::UpdateWaves(const CFPoint& Where)
{
	// For now, defer to legacy wave processing if this class has empty EnemySpawnCountsArray member.
	if(EnemySpawnCountsArray.IsEmpty())
	{
		return;
	}

	if(    !( (this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		   || (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK) ))
	{
		return;
	}

	m_fRepopCounter = 0.0f;

	if(m_nAttackWave >= MaxWaves)
	{
			return;
	}
	
	for(int32 i = 0; i < EnemySpawnCountsArray.Num(); i++)	
	{	
		for(int32 j = 0; j < EnemySpawnCountsArray[i].NumPerWave[m_nAttackWave] && this->HostilesRemaining() > 0; j++)	
		{	
			const float wp = m_pArena->GetWidth();

			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + Where.x;	
			x = (float)fmod(x, wp);	

			float y;	

			switch(EnemySpawnCountsArray[i].Kind)	
			{	
				// Make these enemies spawn high up
				case ObjType::LANDER:	
				case ObjType::BOUNCER:	
					y = FRANDRANGE(0.85f, 1.0f);	
					break;	
					
				default:	
					y = FRANDRANGE(MinSpawnAlt, MaxSpawnAlt);	
			}	

			y *= m_pArena->GetHeight();	

			m_pArena->CreateEnemy(EnemySpawnCountsArray[i].Kind, CFPoint(x, y), FRANDRANGE(0.0f, JFactor * j), true, true);	
		}	
	}	
	
	m_nAttackWave++;
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
					Human->m_pos.Set(FRANDRANGE(0.0f, m_pArena->GetWidth() - 1), 0.04f * m_pArena->GetHeight()); 
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

	float wp = m_pArena->GetWidth();
	float x = (FRAND - 0.5f) * 1.0f * m_pArena->GetDisplayWidth() + where.x;
	x = (float)fmod(x, wp);
	float y = FRANDRANGE(0.3f, 0.8f) * m_pArena->GetHeight();

	m_pArena->CreateEnemy(objType, CFPoint(x, y), 0.0f, true, false);
}


void Defcon::CMilitaryMission::AddBaiter(const CFPoint& where)
{
	m_fLastCleaner = m_fAge;

	float wp = m_pArena->GetWidth();
	float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
	x = (float)fmod(x, wp);
	float y = FRANDRANGE(0.2f, 0.8f) * m_pArena->GetHeight();

	m_pArena->CreateEnemy(ObjType::BAITER, CFPoint(x, y), 0.0f, true, false);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
