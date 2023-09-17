// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight_2.cpp
	"Yllabian Dogfight 2" mission in Defence Condition.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight2::Init()
{
	Super::Init();

	SpawnTimeFactor = 0.1f;

	AddEnemySpawnInfo({EObjType::GUPPY,   { 24, 30, 30, 30 } });
	AddEnemySpawnInfo({EObjType::SWARMER, {  3,  3,  3,  3 } });
	AddEnemySpawnInfo({EObjType::POD,     {  3,  3,  3,  0 } });
	AddEnemySpawnInfo({EObjType::DYNAMO,  { 10,  7,  4,  3 } });
	AddEnemySpawnInfo({EObjType::HUNTER,  {  3,  4,  4,  3 } });

	IntroText =
		"More Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight2::MakeTargets(float DeltaTime, const CFPoint& where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		AddMissionCleaner(where);
	}

	Super::MakeTargets(DeltaTime, CFPoint(GArena->GetWidth() / 2, GArena->GetHeight() / 2));
}


void Defcon::CYllabianDogfight2::OverrideSpawnPoint(EObjType ObjType, CFPoint& SpawnPoint)
{
	// Have all enemies spawn everywhere, except for guppies and swarmers.

	switch(ObjType)
	{
		case EObjType::GUPPY:
		case EObjType::SWARMER:

			SpawnPoint.x = FRANDRANGE(-0.5f, 0.5f) * GArena->GetDisplayWidth() + GArena->GetWidth() / 2;
			break;
	}
}
