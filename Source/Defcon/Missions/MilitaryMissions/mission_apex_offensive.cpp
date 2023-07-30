// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_apex_offensive.cpp
	"Apex Offensive" mission in Defence Condition.
	The landers are given a full escort. This wave 
	marks the style of most military missions.
*/


#include "MilitaryMission.h"



void Defcon::CApexOffensive::Init()
{
	CMilitaryMission::Init();

	IntroText =
		"The landers have adopted an all-out\n"
		"escort strategy. Destroy the landers first\n"
		"to minimize humanoid losses."
		;

	AddEnemySpawnInfo({ ObjType::LANDER,          { 3, 6, 5, 4 } });
	AddEnemySpawnInfo({ ObjType::GUPPY,           { 4, 4, 3, 2 } });
	AddEnemySpawnInfo({ ObjType::HUNTER,          { 0, 1, 2, 1 } });
	AddEnemySpawnInfo({ ObjType::FIREBOMBER_TRUE, { 2, 1, 2, 1 } });
	AddEnemySpawnInfo({ ObjType::FIREBOMBER_WEAK, { 2, 1, 2, 1 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO,          { 3, 3, 2, 1 } });
	AddEnemySpawnInfo({ ObjType::POD,             { 3, 1, 0, 0 } });
	AddEnemySpawnInfo({ ObjType::BOMBER,          { 2, 2, 2, 2 } });
}


void Defcon::CApexOffensive::MakeTargets(float fElapsed, const CFPoint& where)
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

