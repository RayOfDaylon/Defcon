// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_reinforcements.cpp
	"Reinforcements" mission in Defence Condition.
	The landers are escorted by dynamos.
*/



#include "MilitaryMission.h"



void Defcon::CReinforcedMission::Init()
{
	CMilitaryMission::Init();

	IntroText = 
		"More landers are coming.\n"
		"This time, they have dynamo escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;

	AddEnemySpawnInfo({ ObjType::LANDER, { 4, 4, 4 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO, { 3, 2, 1 } });
}


void Defcon::CReinforcedMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
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

