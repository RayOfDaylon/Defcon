// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bomber_showdown.cpp
	"Bomber showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CBomberShowdown::Init()
{
	CMilitaryMission::Init();
	
	IntroText =
		"An armada of bombers has been detected.\n"
		"They will fill the sky with mines;\n"
		"avoid impacting them at all costs.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;

	AddEnemySpawnInfo({ EObjType::BOMBER, { 9, 9, 0, 0 } });
	AddEnemySpawnInfo({ EObjType::POD,    { 3, 3, 0, 0 } });
}


void Defcon::CBomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	UpdateWaves(where);
}

