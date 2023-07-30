// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_ghost.cpp
	"Ghost" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CGhostMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 9 + 40 + 6;

	IntroText =
		"The 'ghost' reformer variant has appeared en masse.\n"
		"They are based on some type of controlled plasma\n"
		"and can break up and reform at will. They are\n"
		"impossible to destroy at close range.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CGhostMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
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
			{ ObjType::DYNAMO, { 2, 5, 2 } },
			{ ObjType::GHOST, { 10, 15, 15 } },
			{ ObjType::REFORMER, { 3, 2, 1 } }
		};

		SPAWN_ENEMIES(0.1f, 0.1f, 0.85f)

		m_nAttackWave++;
	}
}

