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



void Defcon::CMilitaryMission::Init()
{
	IMission::Init();

	// Military missions have stargates.
	this->AddStargate();

#if 0
	// Turrets are an experiment, maybe later we'll have them.
	// Add some turrets.

	check(gpArena != nullptr);

	if(gpArena->HasTerrain())
	{
		for(int i = 0; i < 10; i++)
		{
			//auto pEvt = new CCreateEnemyEvent;
			pEvt->Init(p);
			pEvt->m_objtype = ObjType::TURRET;
			pEvt->m_when = GameTime();
			float wp = gpArena->GetWidth();
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
		NumHostilesRemaining += EnemyTypeCount;

		if(EnemySpawnCounts.Kind == ObjType::LANDER)
		{
			NumLandersRemaining	+= EnemyTypeCount;
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
	check(gpArena != nullptr);
	StargatePtr = new CStargate;
	CFPoint pos(0.5f, 0.75f);
	pos.Mul(CFPoint(gpArena->GetWidth(), gpArena->GetHeight()));
	StargatePtr->m_pos = pos; 
	StargatePtr->InstallSprite();
	gpArena->GetObjects().Add(StargatePtr);

	// Since the stargate is stationary, cache its rectangle.
	StargateRect.Set(StargatePtr->m_pos);
	StargateRect.Inflate(StargatePtr->m_bboxrad);
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

	if(    !( (this->HostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		   || (this->HostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK) ))
	{
		return;
	}

	RepopCounter = 0.0f;

	if(WaveIndex >= MaxWaves)
	{
			return;
	}
	
	for(int32 i = 0; i < EnemySpawnCountsArray.Num(); i++)	
	{	
		for(int32 j = 0; j < EnemySpawnCountsArray[i].NumPerWave[WaveIndex] && this->HostilesRemaining() > 0; j++)	
		{	
			const float wp = gpArena->GetWidth();

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

			y *= gpArena->GetHeight();	

			gpArena->CreateEnemy(EnemySpawnCountsArray[i].Kind, CFPoint(x, y), FRANDRANGE(0.0f, JFactor * j), true, true);	
		}	
	}	
	
	WaveIndex++;
}


bool Defcon::CMilitaryMission::Update(float DeltaTime)
{
	if(!IMission::Update(DeltaTime))
	{
		return false;
	}

	RepopCounter += DeltaTime;

	if(this->PlayerInStargate())
	{
		gpArena->AllStopPlayerShip();

		if(IsCompleted())
		{
			// Warp to next mission.
			auto& Player = gpArena->GetPlayerShip();
			Player.EnableInput(false);

			// If the player is carrying any humans, redistribute them back to the ground to avoid clumping in next mission.
			gDefconGameInstance->GetHumans().ForEach([this](IGameObject* pObj)
			{
				auto Human = static_cast<CHuman*>(pObj);

				if(Human->IsBeingCarried() && Human->GetCarrier()->GetType() == ObjType::PLAYER)
				{
					Human->SetToNotCarried();
					Human->m_pos.Set(FRANDRANGE(0.0f, gpArena->GetWidth() - 1), 0.04f * gpArena->GetHeight()); 
				}
			});
 			return false;
		}

		gpArena->TransportPlayerShip();
	}

	if(this->HostilesRemaining() > 0)
	{
		this->MakeTargets(DeltaTime, gpArena->GetPlayerShip().m_pos);
	}

	return true;
}


int32 Defcon::CMilitaryMission::HostilesRemaining() const
{
	return NumHostilesRemaining;
}


int32 Defcon::CMilitaryMission::HostilesInPlay() const
{
	return (int32)gpArena->GetEnemies().Count();
}


void Defcon::CMilitaryMission::AddMissionCleaner(const CFPoint& where)
{
	// When a mission drags on too long, it's time 
	// to throw in various nonmission hostiles at an increasing 
	// frequency until the player wipes out the critical opponents.

	const ObjType MunchieTypes[] = { ObjType::PHRED, ObjType::BIGRED, ObjType::MUNCHIE };

	if(Age - TimeLastCleanerSpawned >= CleanerFreq)
	{
		if(CleanerFreq > 0.5f)
		{
			CleanerFreq -= 0.5f;
		}

		this->AddNonMissionTarget(FRAND < BAITER_PROB ? ObjType::BAITER : MunchieTypes[IRAND(3)], where);
	}
}


void Defcon::CMilitaryMission::AddNonMissionTarget(ObjType objType, const CFPoint& where)
{
	TimeLastCleanerSpawned = Age;

	float wp = gpArena->GetWidth();
	float x = (FRAND - 0.5f) * 1.0f * gpArena->GetDisplayWidth() + where.x;
	x = (float)fmod(x, wp);
	float y = FRANDRANGE(0.3f, 0.8f) * gpArena->GetHeight();

	gpArena->CreateEnemy(objType, CFPoint(x, y), 0.0f, true, false);
}


void Defcon::CMilitaryMission::AddBaiter(const CFPoint& where)
{
	TimeLastCleanerSpawned = Age;

	float wp = gpArena->GetWidth();
	float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
	x = (float)fmod(x, wp);
	float y = FRANDRANGE(0.2f, 0.8f) * gpArena->GetHeight();

	gpArena->CreateEnemy(ObjType::BAITER, CFPoint(x, y), 0.0f, true, false);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
