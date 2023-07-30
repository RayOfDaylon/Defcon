// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_reformer_showdown.cpp
	"Reformer showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CReformerShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 6+6;

	IntroText =
		"Reformers have moved in to engage you.\n"
		"Destroy their parts before they regroup.\n"
		"Also watch out for swarmer escorts.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CReformerShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 2)
			return;

		const Wave waves[] =
		{
			{ ObjType::REFORMER, { 4, 2, 0, 0 } },
			{ ObjType::SWARMER,  { 3, 3, 0, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

