// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_apex_offensive_lite.cpp
	"Apex Offensive" mission in Defence Condition.
	The landers are given a medium escort. This wave 
	marks the style of most military missions.
*/


#include "MilitaryMission.h"



void Defcon::CApexOffensiveLite::Init()
{
	CMilitaryMission::Init();
	
	IntroText =
		"The landers have adopted a more varied\n"
		"escort strategy. Destroy the landers first\n"
		"to minimize humanoid losses."
	;

	AddEnemySpawnInfo({ ObjType::LANDER,          { 3, 6, 5, 4 } });
	AddEnemySpawnInfo({ ObjType::GUPPY,           { 2, 2, 3, 2 } });
	AddEnemySpawnInfo({ ObjType::HUNTER,          { 0, 1, 1, 1 } });
	AddEnemySpawnInfo({ ObjType::FIREBOMBER_TRUE, { 1, 1, 1, 1 } });
	AddEnemySpawnInfo({ ObjType::FIREBOMBER_WEAK, { 1, 1, 1, 1 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO,          { 2, 2, 2, 1 } });
	AddEnemySpawnInfo({ ObjType::POD,             { 2, 1, 0, 0 } });
	AddEnemySpawnInfo({ ObjType::BOMBER,          { 2, 2, 1, 0 } });
}


void Defcon::CApexOffensiveLite::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 4
			)
	{
		// Add baiters until player clears minimal hostiles.
		if(Age - TimeLastCleanerSpawned >= BAITER_SPAWN_FREQUENCY)
		{
			this->AddBaiter(where);
		}
	}

	UpdateWaves(where);
}

