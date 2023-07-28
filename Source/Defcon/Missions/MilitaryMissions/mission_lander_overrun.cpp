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
}


bool Defcon::CLanderOverrun::Update(float fElapsed)
{
	// - Show some intro text.
	// - Make nine landers appear, first wave
	// - of four, second of three, third of two.
	//   Make them come in every DELAY_BETWEEN_REATTACK 
	//   or if no hostiles exist.
	// - When all hostiles gone,
	//   the mission ends.

	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CLanderOverrun::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"A large mob of landers with minimal escort",
		"has panicked and decided to attack on their own.",
		"",
		"Engage enemy forces and eliminate."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
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

