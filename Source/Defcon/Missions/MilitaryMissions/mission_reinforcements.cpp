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
	Super::Init();

	IntroText = 
		"More landers are coming.\n"
		"This time, they have dynamo escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;

	AddEnemySpawnInfo({ EObjType::LANDER, { 4, 4, 4 } });
	AddEnemySpawnInfo({ EObjType::DYNAMO, { 3, 2, 1 } });
}


void Defcon::CReinforcedMission::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		// Add baiters until player clears minimal hostiles.
		if(Age - TimeLastCleanerSpawned >= BAITER_SPAWN_FREQUENCY)
		{
			AddBaiter(Where);
		}
	}

	Super::MakeTargets(DeltaTime, Where);
}

