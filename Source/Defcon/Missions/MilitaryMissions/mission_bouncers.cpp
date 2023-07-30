// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bouncers.cpp
	Bouncers, landers, firebombers, and bombers.
*/


#include "MilitaryMission.h"




void Defcon::CBouncersMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 71 + 9 + 9;
	m_nLandersRemaining  = 30;

	IntroText = "A group of bouncers has decided to 'drop' in.";
}


void Defcon::CBouncersMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		this->AddMissionCleaner(where);
	}


	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] = 
		{
			{ ObjType::LANDER, { 10, 10, 10, } },
			{ ObjType::BOMBER, { 3, 5, 3 } },
			{ ObjType::BOUNCER_TRUE, { 5, 5, 5 } }, 
			{ ObjType::BOUNCER_WEAK, { 3, 3, 3 } }, 
			{ ObjType::FIREBOMBER_TRUE, { 5, 5, 5 } },
			{ ObjType::FIREBOMBER_WEAK, { 3, 3, 3 } }
		};

		STANDARD_ENEMY_SPAWNING(0.1f)

		m_nAttackWave++;
	}
}

