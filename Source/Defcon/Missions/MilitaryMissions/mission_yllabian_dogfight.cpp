// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight.cpp
	"Yllabian Dogfight" mission in Defence Condition.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight::Init()
{
	CMilitaryMission::Init();

	MaxWaves = 3;

	NumTargetsRemaining = 12 + 15 + 9 + 6 + 3 + 6;

	IntroText =
		"Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(WaveIndex >= MaxWaves)
	{
		return;
	}

	if((TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (TotalHostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		const FEnemySpawnCounts SpawnCounts[] = 
		{
			{ EObjType::GUPPY,   { 12, 15, 9, 0 } },
			{ EObjType::SWARMER, {  6,  3, 6, 0 } }
		};


		const float ArenaWidth = GArena->GetWidth();

		int32 i, j;

		for(i = 0; i < array_size(SpawnCounts); i++)
		{
			for(j = 0; j < SpawnCounts[i].NumPerWave[WaveIndex]; j++)
			{
				float x = FRANDRANGE(-0.5f, 0.5f) * GArena->GetDisplayWidth() + ArenaWidth / 2;
				x = (float)fmod(x, ArenaWidth);

				const float y = FRANDRANGE(0.25f, 0.75f) * GArena->GetHeight();

				GArena->CreateEnemy(SpawnCounts[i].Kind, EObjType::UNKNOWN, CFPoint(x, y), FRANDRANGE(0.0f, 0.1f * j), EObjectCreationFlags::StandardEnemy);
			}
		}

		WaveIndex++;
	}
}

