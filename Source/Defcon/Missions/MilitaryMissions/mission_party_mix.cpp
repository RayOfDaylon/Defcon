// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_random.cpp
	"Party Mix" mission in Defence Condition.
	The enemy types are chosen totally at random.

	Copyright 2004 Daylon Graphics Ltd.
*/


#include "MilitaryMission.h"




void Defcon::CPartyMixMission::Init()
{
	Super::Init();

	IntroText =
		"The Apex are temporarily thrown into chaos\n"
		"and can only mount a mixed, random counterattack.\n"
		"There's no telling what will come your way."
		;


	// Build up a local enemy counts array that we can then copy to the actual one.

	const int32 EnemyCounts[] = { 15, 20, 25, 20 };

	TArray<FEnemySpawnCounts> LocalSpawnCounts;

	for(int32 I = 0; I < array_size(EnemyTypes); I++)
	{
		LocalSpawnCounts.Add({ EnemyTypes[I], {} });
	}

	for(int32 AttackWave = 0; AttackWave < 4; AttackWave++)
	{
		for(int32 j = 0; j < EnemyCounts[AttackWave]; j++)
		{
			const auto Kind = EnemyTypes[IRAND(array_size(EnemyTypes))];

			for(int32 k = 0; k < LocalSpawnCounts.Num(); k++)
			{
				if(LocalSpawnCounts[k].Kind == Kind)
				{
					LocalSpawnCounts[k].NumPerWave[AttackWave]++;
					break;
				}
			}
		}
	}

	for(const auto& LocalSpawnCount : LocalSpawnCounts)
	{
		AddEnemySpawnInfo(LocalSpawnCount);
	}
}


void Defcon::CPartyMixMission::MakeTargets(float DeltaTime, const CFPoint& Where)
{
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		AddMissionCleaner(Where);
	}

	Super::MakeTargets(DeltaTime, Where);
}

