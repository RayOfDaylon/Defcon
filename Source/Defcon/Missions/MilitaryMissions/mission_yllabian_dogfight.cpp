// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight.cpp
	"Yllabian Dogfight" mission in Defence Condition.

	Copyright 2004 Daylon Graphics Ltd.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight::Init()
{
	CMilitaryMission::Init();

	NumHostilesRemaining = 12+15+9+6+3+6;

	IntroText =
		"Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		if(WaveIndex >= 3)
			return;

		const FEnemySpawnCounts waves[] = 
		{
			{ EObjType::GUPPY,   { 12, 15, 9, 0 } },
			{ EObjType::SWARMER, {  6,  3, 6, 0 } }
		};


		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].NumPerWave[WaveIndex] && this->HostilesRemaining() > 0; j++)
			{
				float wp = gpArena->GetWidth();
				float x = (FRAND - 0.5f) * gpArena->GetDisplayWidth() + wp/2;
				x = (float)fmod(x, wp);
				float y = FRANDRANGE(0.25f, 0.75f) * gpArena->GetHeight();

				gpArena->CreateEnemy(waves[i].Kind, CFPoint(x, y), FRANDRANGE(0.0f, 0.1f * j), true, true);
			}
		}

		WaveIndex++;
	}
}

