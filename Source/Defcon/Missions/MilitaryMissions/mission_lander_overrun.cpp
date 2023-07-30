// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_lander_overrun.cpp
	"Lander Overrun" mission in Defence Condition.
	A ton of landers with minimal escort.

	Copyright 2004 Daylon Graphics Ltd.
*/



#include "MilitaryMission.h"



void Defcon::CLanderOverrun::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 50*2 + 6*2;
	m_nLandersRemaining  = 100;

	IntroText = 
		"A large mob of landers with minimal escort\n"
		"has panicked and decided to attack on their own.\n"
		"\n"
		"Engage enemy forces and eliminate."
		;
}


void Defcon::CLanderOverrun::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| m_fRepopCounter > DELAY_BETWEEN_REATTACK)
	{
		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 2)
			return;

		const Wave waves[] = 
		{
			{ ObjType::LANDER, { 50, 50, 0, 0 } },
			{ ObjType::DYNAMO, { 6, 6, 0, 0 } },
		};

		STANDARD_ENEMY_SPAWNING(0.1f)

		m_nAttackWave++;
	}
}

