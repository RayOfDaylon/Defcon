// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_lander_overrun.cpp
	"Lander Overrun" mission in Defence Condition.
	A ton of landers with minimal escort.
*/


#include "MilitaryMission.h"


void Defcon::CLanderOverrun::Init()
{
	CMilitaryMission::Init();

	IntroText = 
		"A large mob of landers with minimal escort\n"
		"has panicked and decided to attack on their own.\n"
		"\n"
		"Engage enemy forces and eliminate."
		;

	AddEnemySpawnInfo({ EObjType::LANDER, { 50, 50 } });
	AddEnemySpawnInfo({ EObjType::DYNAMO, {  6,  6 } });
}


void Defcon::CLanderOverrun::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	UpdateWaves(Where);
}

