// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bouncers.cpp
	Bouncers, landers, firebombers, and bombers.
*/


#include "MilitaryMission.h"




void Defcon::CBouncersMission::Init()
{
	CMilitaryMission::Init();

	IntroText = "A group of bouncers has decided to 'drop' in.";

	JFactor = 0.1f;

	AddEnemySpawnInfo({ EObjType::LANDER,          { 10, 10, 10, } });
	AddEnemySpawnInfo({ EObjType::BOMBER,          {  3,  5,  3  } });
	AddEnemySpawnInfo({ EObjType::BOUNCER_TRUE,    {  5,  5,  5  } });
	AddEnemySpawnInfo({ EObjType::BOUNCER_WEAK,    {  3,  3,  3  } });
	AddEnemySpawnInfo({ EObjType::FIREBOMBER_TRUE, {  5,  5,  5  } });
	AddEnemySpawnInfo({ EObjType::FIREBOMBER_WEAK, {  3,  3,  3  } });
}


void Defcon::CBouncersMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		this->AddMissionCleaner(where);
	}

	UpdateWaves(where);
}

