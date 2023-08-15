// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight_2.cpp
	"Yllabian Dogfight 2" mission in Defence Condition.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight2::Init()
{
	CMilitaryMission::Init();

	MaxWaves = 4;

	NumTargetsRemaining   = 24 + 30 + 30 + 30 +
							  3 +  3 +  3 +  3  +
							  3 +  3 +  3      +
							 10 +  7 +  4 +  3  +
							  3 +  4 +  4 +  3  ;

	IntroText =
		"More Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight2::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		AddMissionCleaner(where);
	}


	if((TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (TotalHostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		const FEnemySpawnCounts SpawnCounts[] = 
		{
			{ EObjType::GUPPY,   { 24, 30, 30, 30 } },
			{ EObjType::SWARMER, {  3,  3,  3,  3 } },
			{ EObjType::POD,     {  3,  3,  3,  0 } },
			{ EObjType::DYNAMO,  { 10,  7,  4,  3 } },
			{ EObjType::HUNTER,  {  3,  4,  4,  3 } }
		};

		const float ArenaWidth = GArena->GetWidth();

		int32 i, j;
		for(i = 0; i < array_size(SpawnCounts); i++)
		{
			for(j = 0; j < SpawnCounts[i].NumPerWave[WaveIndex]; j++)
			{
				CFPoint P;

				switch(SpawnCounts[i].Kind)
				{
					case EObjType::POD:
					case EObjType::HUNTER:
					case EObjType::DYNAMO:
						P.x = FRAND * (GArena->GetWidth() - 1);
						P.y = FRANDRANGE(0.15f, 0.85f);
						break;

					default:
						P.x = (FRAND - 0.5f) * GArena->GetDisplayWidth() + ArenaWidth / 2;
						P.y = FRANDRANGE(0.25f, 0.75f);
						break;
				}

				P.x = (float)fmod(P.x, ArenaWidth);
				P.y *= GArena->GetHeight();

				GArena->CreateEnemy(SpawnCounts[i].Kind, EObjType::UNKNOWN, P, FRANDRANGE(0.0f, 0.1f * j), EObjectCreationFlags::StandardEnemy);
			}
		}

		WaveIndex++;
	}
}

