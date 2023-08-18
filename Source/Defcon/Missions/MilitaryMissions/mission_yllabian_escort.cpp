// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_escort.cpp
	"Yllabian Dogfight 2" but in orbit.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianEscort::Init()
{
	Super::Init();

	IntroText = 
		"Yllabian fighters are escorting a lander attack.\n"
		"Engage enemy forces and eliminate them."
	;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ EObjType::GUPPY,   { 24, 30, 30, 30 } });
	AddEnemySpawnInfo({ EObjType::SWARMER, {  3,  3,  3,  3 } });
	AddEnemySpawnInfo({ EObjType::POD,     {  3,  3,  3,  0 } });
	AddEnemySpawnInfo({ EObjType::DYNAMO,  { 10,  7,  4,  3 } });
	AddEnemySpawnInfo({ EObjType::HUNTER,  {  3,  4,  4,  3 } });
	AddEnemySpawnInfo({ EObjType::LANDER,  { 10, 12, 10,  5 } });
}


void Defcon::CYllabianEscort::MakeTargets(float DeltaTime, const CFPoint& Where)
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
