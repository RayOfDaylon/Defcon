// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_pack.cpp
	A small, dense group of firebombers on the opposite 
	side of the planet.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberPack::Init()
{
	Super::Init();

	JFactor = 0.5f;

	SpawnRangeHorizontal = GArena->GetDisplayWidth();

	AddEnemySpawnInfo({EObjType::FIREBOMBER_TRUE, { 10, 3, 0, 0 } });
	AddEnemySpawnInfo({EObjType::FIREBOMBER_WEAK, { 10, 3, 0, 0 } });
	AddEnemySpawnInfo({EObjType::DYNAMO,          { 10, 5, 0, 0 } });

	IntroText = 
		"A rogue group of firebombers has tightly\n"
		"clustered on the far side of the planet.\n"
		"\n"
		"Their combined firepower in tight quarters\n"
		"may require deploying smartbombs.\n"
	;
}


void Defcon::CFirebomberPack::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	Super::MakeTargets(DeltaTime, CFPoint(GArena->GetWidth() / 2, GArena->GetHeight() / 2));
}

