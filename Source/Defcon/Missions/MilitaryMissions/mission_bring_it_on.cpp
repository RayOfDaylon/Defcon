// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bring_it_on.cpp.cpp
	"Bring it On" mission in Defence Condition.
	This calls for destroying several dynamos 
	and a few reformers while baiters arrive early.
*/


#include "MilitaryMission.h"


void Defcon::CBringItOn::Init()
{
	CMilitaryMission::Init();

	IntroText =
		"Reformers and dynamos have laid a trap.\n"
		"They have signalled baiters, phreds and munchies\n"
		"to arrive early in a high-numbered ambush.\n"
		"\n"
		"Don't let the ambush distract you from\n"
		"properly eliminating the ordinary enemies,\n"
		"otherwise you will be overwhelmed.\n"
	;

	AddEnemySpawnInfo({ EObjType::DYNAMO,   { 12, 0, 0, 0 }});
	AddEnemySpawnInfo({ EObjType::REFORMER, {  6, 0, 0, 0 }});
}


void Defcon::CBringItOn::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 && Age >= PLAYER_BIRTHDURATION + 6)
	{
		this->AddMissionCleaner(where);
	}

	UpdateWaves(where);
}

