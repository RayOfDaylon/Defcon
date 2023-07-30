// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_reinforcements.cpp
	"Reinforcements" mission in Defence Condition.
	The landers are escorted by dynamos.
*/



#include "MilitaryMission.h"



void Defcon::CReinforcedMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 12 + 6;
	m_nLandersRemaining  = 12;
	// 3*4 landers + 6 dynamos

	IntroText = 
		"More landers are coming.\n"
		"This time, they have dynamo escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CReinforcedMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		// Add baiters until player clears minimal hostiles.
		if(m_fAge - m_fLastCleaner >= 2.0f)
		{
			this->AddBaiter(where);
		}
	}


	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] =
		{
			{ ObjType::LANDER, { 4, 4, 4, 0 } },
			{ ObjType::DYNAMO, { 3, 2, 1, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

