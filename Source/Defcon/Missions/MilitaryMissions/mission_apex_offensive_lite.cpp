// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_apex_offensive_lite.cpp
	"Apex Offensive" mission in Defence Condition.
	The landers are given a medium escort. This wave 
	marks the style of most military missions.
*/


#include "MilitaryMission.h"



void Defcon::CApexOffensiveLite::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 18 + 9 + 3 + 4 + 4 + 7 + 3 + 5;
	m_nLandersRemaining  = 3 + 6 + 5 + 4;

	IntroText =
		"The landers have adopted a more varied\n"
		"escort strategy. Destroy the landers first\n"
		"to minimize humanoid losses."
	;
}


void Defcon::CApexOffensiveLite::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 4
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

		if(m_nAttackWave >= 4)
			return;

		const Wave waves[] = 
		{
			{ ObjType::LANDER, { 3, 6, 5, 4 } },
			{ ObjType::GUPPY, { 2, 2, 3, 2 } },
			{ ObjType::HUNTER, { 0, 1, 1, 1 } },
			{ ObjType::FIREBOMBER_TRUE, { 1, 1, 1, 1 } },
			{ ObjType::FIREBOMBER_WEAK, { 1, 1, 1, 1 } },
			{ ObjType::DYNAMO, { 2, 2, 2, 1 } },
			{ ObjType::POD, { 2, 1, 0, 0 } },
			{ ObjType::BOMBER, { 2, 2, 1, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

