// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_showdown.cpp
	"Firebomber showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	//m_nHostilesRemaining = 12 + 6;
	// 6*2 firebombers + 6 dynamos

	IntroText = 
		"Firebombers have moved in to engage you.\n"
		"Try to fire at them from a distance.\n"
		"Also watch out for dynamo escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them.\n"
		;

	AddEnemySpawnInfo({ ObjType::FIREBOMBER_TRUE, { 3, 3, 0, 0 } });
	AddEnemySpawnInfo({ ObjType::FIREBOMBER_WEAK, { 3, 3, 0, 0 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO,          { 3, 3, 0, 0 } });
}


void Defcon::CFirebomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	UpdateWaves(where);

#if 0
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;
		const int32 numBombers[] = { 6, 6 }; 
		const int32 numEscorts[] = { 3, 3 }; 

		if(m_nAttackWave >= array_size(numBombers))
			return;

		const Wave waves[] =
		{
			{ ObjType::FIREBOMBER_TRUE, { 3, 3, 0, 0 } },
			{ ObjType::FIREBOMBER_WEAK, { 3, 3, 0, 0 } },
			{ ObjType::DYNAMO,          { 3, 3, 0, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f);

		m_nAttackWave++;
	}
#endif
}

