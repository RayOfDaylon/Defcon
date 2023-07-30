// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_reformer_showdown.cpp
	"Reformer showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CReformerShowdown::Init()
{
	CMilitaryMission::Init();

	IntroText =
		"Reformers have moved in to engage you.\n"
		"Destroy their parts before they regroup.\n"
		"Also watch out for swarmer escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;

	AddEnemySpawnInfo({ ObjType::REFORMER, { 4, 2 } });
	AddEnemySpawnInfo({ ObjType::SWARMER,  { 3, 3 } });
}


void Defcon::CReformerShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	UpdateWaves(where);
}

