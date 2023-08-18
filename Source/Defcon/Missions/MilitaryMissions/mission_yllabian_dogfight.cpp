// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight.cpp
	"Yllabian Dogfight" mission in Defence Condition.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight::Init()
{
	Super::Init();

	//MaxWaves = 3;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ EObjType::GUPPY,   { 12, 15, 9 } });
	AddEnemySpawnInfo({ EObjType::SWARMER, {  6,  3, 6 } });

	SpawnRangeHorizontal = GArena->GetDisplayWidth();

	IntroText =
		"Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	Super::MakeTargets(DeltaTime, CFPoint(GArena->GetWidth() / 2, GArena->GetHeight() / 2));
}

