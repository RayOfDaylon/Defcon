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
#include "GameObjects/playership.h"
#include "GameObjects/human.h"
#include "GameObjects/Enemies/enemies.h"
#include "GameObjects/Auxiliary/stargate.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


Defcon::CMilitaryMission::CMilitaryMission()
{
}


void Defcon::CMilitaryMission::Init()
{
	Super::Init();

	// Military missions have stargates.
	AddStargate();

	SpawnRangeHorizontal = GArena->GetWidth() * ATTACK_INITIALDISTANCE;

#if 0
	// Turrets are an experiment, maybe later we'll have them.
	// Add some turrets.

	check(gpArena != nullptr);

	if(gpArena->HasTerrain())
	{
		for(int32 i = 0; i < 10; i++)
		{
			//auto pEvt = new CCreateEnemyTask;
			pEvt->Init(p);
			pEvt->EnemyType = EObjType::TURRET;
			pEvt->Countdown = 0.0f;
			float ArenaWidth = gpArena->GetWidth();
			float X = FRAND * ArenaWidth;
			X = (float)fmod(X, ArenaWidth);
			float Y = p->GetTerrainElev(X) - 10;
			pEvt->Where.Set(X, Y);
			pEvt->bMissionTarget = false; // can leave turrets alive w/o aborting mission
			pEvt->m_bMaterializes = false;
			AddTask(pEvt);
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
		NumTargetsRemaining += EnemyTypeCount;

		if(EnemySpawnCounts.Kind == EObjType::LANDER)
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
	check(GArena != nullptr);
	StargatePtr = new CStargate;
	CFPoint pos(0.5f, 0.75f);
	pos.Mul(CFPoint(GArena->GetWidth(), GArena->GetHeight()));
	StargatePtr->Position = pos; 
	StargatePtr->InstallSprite();
	GArena->GetObjects().Add(StargatePtr);

	// Since the stargate is stationary, cache its rectangle.
	StargateRect.Set(StargatePtr->Position);
	StargateRect.Inflate(StargatePtr->BboxRadius);
}


bool Defcon::CMilitaryMission::PlayerInStargate() const
{
	return StargateRect.PtInside(GArena->GetPlayerShip().Position);
}


void Defcon::CMilitaryMission::UpdateWaves(const CFPoint& Where)
{
	// For now, defer to legacy wave processing if this class has empty EnemySpawnCountsArray member.

	if(WaveIndex >= MaxWaves || EnemySpawnCountsArray.IsEmpty())
	{
		return;
	}

	if(    !( (TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		   || (TotalHostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK) ))
	{
		return;
	}

	RepopCounter = 0.0f;
	
	const float ArenaWidth = GArena->GetWidth();

	for(int32 I = 0; I < EnemySpawnCountsArray.Num(); I++)	
	{	
		for(int32 J = 0; J < EnemySpawnCountsArray[I].NumPerWave[WaveIndex]; J++)	
		{	
			CFPoint SpawnPoint;

			SpawnPoint.x = FRANDRANGE(-0.5f, 0.5f) * SpawnRangeHorizontal + Where.x;	
			SpawnPoint.x = (float)fmod(SpawnPoint.x, ArenaWidth);	

			switch(EnemySpawnCountsArray[I].Kind)	
			{	
				// Make these enemies spawn in upper half
				case EObjType::LANDER:	
				case EObjType::BOUNCER:	
					SpawnPoint.y = FRANDRANGE(0.5f, 0.95f);	
					break;	
					
				default:	
					SpawnPoint.y = FRANDRANGE(MinSpawnAlt, MaxSpawnAlt);	
			}	

			SpawnPoint.y *= GArena->GetHeight();

			OverrideSpawnPoint(EnemySpawnCountsArray[I].Kind, SpawnPoint);

			GArena->CreateEnemy(EnemySpawnCountsArray[I].Kind, EObjType::UNKNOWN, SpawnPoint, FRANDRANGE(0.0f, JFactor * J), EObjectCreationFlags::StandardEnemy);	
		}	
	}	
	
	WaveIndex++;
}


bool Defcon::CMilitaryMission::IsComplete() const
{
	// Report if the mission has ended and the player is allowed to leave.
	// All the waves must have occured, no more mission target creation tasks are pending,
	// and none of the existing enemies must be a mission target.

	if(WaveIndex < MaxWaves)
	{
		return false;
	}


	bool Result = true;

	EnemyCreationTasks.ForEachUntil([&Result](CScheduledTask* Task)
	{
		auto EnemyCreationTask = static_cast<CCreateEnemyTask*>(Task);
		
		if(!EnemyCreationTask->bMissionTarget)
		{
			return true;
		}
		Result = false;
		return false;
	});

	if(Result == false)
	{
		return false;
	}


	Result = true;

	GArena->GetEnemies().ForEachUntil([&Result](IGameObject* Obj)
	{
		const auto Enemy = static_cast<CEnemy*>(Obj);

		if(!Enemy->IsMissionTarget())
		{
			return true;
		}

		Result = false;
		return false;
	});

	return Result;
}


bool Defcon::CMilitaryMission::Update(float DeltaTime)
{
	if(!Super::Update(DeltaTime))
	{
		return false;
	}

	RepopCounter += DeltaTime;

	if(PlayerInStargate())
	{
		GArena->AllStopPlayerShip();

		if(!IsComplete())
		{
			GArena->TransportPlayerShip();
		}
		else
		{
			// Warp to next mission.
			auto& Player = GArena->GetPlayerShip();
			Player.EnableInput(false);

			// If the player is carrying any humans, redistribute them back to the ground to avoid clumping in next mission.
			GDefconGameInstance->GetHumans().ForEach([this](IGameObject* pObj)
			{
				auto Human = static_cast<CHuman*>(pObj);

				if(Human->IsBeingCarried() && Human->GetCarrier()->GetType() == EObjType::PLAYER)
				{
					Human->SetToNotCarried();
					Human->Position.Set(FRANDRANGE(0.0f, GArena->GetWidth() - 1), 0.04f * GArena->GetHeight()); 
				}
			});

 			return false;
		}
	}

	MakeTargets(DeltaTime, GArena->GetPlayerShip().Position);

	return true;
}


void Defcon::CMilitaryMission::TargetDestroyed(EObjType Kind)
{
	check(NumTargetsRemaining > 0);

	NumTargetsRemaining--;

	if(Kind == EObjType::LANDER) 
	{
		check(NumLandersRemaining > 0);

		NumLandersRemaining--; 
	} 
}


int32 Defcon::CMilitaryMission::TargetsRemaining() const
{
	return NumTargetsRemaining;
}


int32 Defcon::CMilitaryMission::TotalHostilesInPlay() const
{
	check(GArena != nullptr);

	return (int32)GArena->GetEnemies().Count();
}


void Defcon::CMilitaryMission::AddMissionCleaner(const CFPoint& where)
{
	// When a mission drags on too long, it's time 
	// to throw in various nonmission hostiles at an increasing 
	// frequency until the player wipes out the critical opponents.

	const EObjType MunchieTypes[] = { EObjType::PHRED, EObjType::BIGRED, EObjType::MUNCHIE };

	if(Age - TimeLastCleanerSpawned >= CleanerFreq)
	{
		if(CleanerFreq > 0.5f)
		{
			CleanerFreq -= 0.5f;
		}

		AddNonTarget(FRAND < BAITER_PROB ? EObjType::BAITER : MunchieTypes[IRAND(3)], where);
	}
}


void Defcon::CMilitaryMission::AddNonTarget(EObjType ObjType, const CFPoint& Where)
{
	check(GArena != nullptr);

	TimeLastCleanerSpawned = Age;

	const float ArenaWidth = GArena->GetWidth();
	float X = (FRAND - 0.5f) * 1.0f * GArena->GetDisplayWidth() + Where.x;
	X = (float)fmod(X, ArenaWidth);
	float Y = FRANDRANGE(0.3f, 0.8f) * GArena->GetHeight();

	GArena->CreateEnemy(ObjType, EObjType::UNKNOWN, CFPoint(X, Y), 0.0f, EObjectCreationFlags::CleanerEnemy);
}


void Defcon::CMilitaryMission::AddBaiter(const CFPoint& Where)
{
	TimeLastCleanerSpawned = Age;

	const float ArenaWidth = GArena->GetWidth();
	float X = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * ArenaWidth + Where.x;
	X = (float)fmod(X, ArenaWidth);
	float Y = FRANDRANGE(0.2f, 0.8f) * GArena->GetHeight();

	GArena->CreateEnemy(EObjType::BAITER, EObjType::UNKNOWN, CFPoint(X, Y), 0.0f, EObjectCreationFlags::CleanerEnemy);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
