// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_ghost.cpp
	"Ghost" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CGhostMission::Init()
{
	Super::Init();

	IntroText =
		"The 'ghost' reformer variant has appeared en masse.\n"
		"They are based on some type of controlled plasma\n"
		"and can break up and reform at will. They are\n"
		"impossible to destroy at close range.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;

	SpawnTimeFactor = 0.1f;

	AddEnemySpawnInfo({ EObjType::DYNAMO,   {  2,  5,  2 } });
	AddEnemySpawnInfo({ EObjType::GHOST,    { 10, 15, 15 } });
	AddEnemySpawnInfo({ EObjType::REFORMER, {  3,  2,  1 } });
}


void Defcon::CGhostMission::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		AddMissionCleaner(Where);
	}

	Super::MakeTargets(DeltaTime, Where);
}

