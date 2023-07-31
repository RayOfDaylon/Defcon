// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_showdown.cpp
	"Firebomber showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberShowdown::Init()
{
	CMilitaryMission::Init();

	IntroText = 
		"Firebombers have moved in to engage you.\n"
		"Try to fire at them from a distance.\n"
		"Also watch out for dynamo escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them.\n"
		;

	AddEnemySpawnInfo({ EObjType::FIREBOMBER_TRUE, { 3, 3, 0, 0 } });
	AddEnemySpawnInfo({ EObjType::FIREBOMBER_WEAK, { 3, 3, 0, 0 } });
	AddEnemySpawnInfo({ EObjType::DYNAMO,          { 3, 3, 0, 0 } });
}


void Defcon::CFirebomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	UpdateWaves(where);
}

