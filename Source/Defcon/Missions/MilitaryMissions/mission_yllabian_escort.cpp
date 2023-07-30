// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_escort.cpp
	"Yllabian Dogfight 2" but in orbit.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianEscort::Init()
{
	CMilitaryMission::Init();

	IntroText = 
		"Yllabian fighters are escorting a lander attack.\n"
		"Engage enemy forces and eliminate them."
	;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ ObjType::GUPPY,   { 24, 30, 30, 30 } });
	AddEnemySpawnInfo({ ObjType::SWARMER, {  3,  3,  3,  3 } });
	AddEnemySpawnInfo({ ObjType::POD,     {  3,  3,  3,  0 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO,  { 10,  7,  4,  3 } });
	AddEnemySpawnInfo({ ObjType::HUNTER,  {  3,  4,  4,  3 } });
	AddEnemySpawnInfo({ ObjType::LANDER,  { 10, 12, 10,  5 } });
}


void Defcon::CYllabianEscort::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		this->AddMissionCleaner(where);
	}

	UpdateWaves(where);
}
