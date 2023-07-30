// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bomber_showdown.cpp
	"Bomber showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CBomberShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 18+6;
	// 9*2 bombers + 3*2 pods

	IntroText =
		"An armada of bombers has been detected.\n"
		"They will fill the sky with mines;\n"
		"avoid impacting them at all costs.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CBomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 2)
		{
			return;
		}

		const Wave waves[] = 
		{
			{ ObjType::BOMBER, { 9, 9, 0, 0 } },
			{ ObjType::POD,    { 3, 3, 0, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

