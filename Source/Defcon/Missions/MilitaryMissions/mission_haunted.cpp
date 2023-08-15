// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_haunted.cpp
	Ghosts and landers mission in Defence Condition.
*/


#include "MilitaryMission.h"


void Defcon::CHaunted::Init()
{
	CMilitaryMission::Init();

	IntroText =
		"Landers are arriving with a ghost escort.\n"
		"Engage enemy forces and eliminate."
		;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ EObjType::LANDER, { 10, 10, 10 } });
	AddEnemySpawnInfo({ EObjType::DYNAMO, {  3,  5,  3 } });
	AddEnemySpawnInfo({ EObjType::GHOST,  { 10, 15, 15 } });
}


void Defcon::CHaunted::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		AddMissionCleaner(Where);
	}

	Super::MakeTargets(DeltaTime, Where);
}

