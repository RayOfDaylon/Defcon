// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_bring_it_on.cpp.cpp
	"Bring it On" mission in Defence Condition.
	This calls for destroying several dynamos 
	and a few reformers while baiters arrive early.
*/


#include "MilitaryMission.h"


void Defcon::CBringItOn::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 12 + 6;

	IntroText =
		"Reformers and dynamos have laid a trap.\n"
		"They have signalled baiters, phreds and munchies\n"
		"to arrive early in a high-numbered ambush.\n"
		"\n"
		"Don't let the ambush distract you from\n"
		"properly eliminating the ordinary enemies,\n"
		"otherwise you will be overwhelmed.\n"
	;
}


void Defcon::CBringItOn::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 && m_fAge >= PLAYER_BIRTHDURATION + 6)
	{
		this->AddMissionCleaner(where);
	}


	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;
		//const int32 numDynamos[] = { 12 }; 
		//const int32 numReformers[] = { 6 }; 
		
		if(m_nAttackWave >= 1)//array_size(numDynamos))
			return;

		const Wave waves[] = { 
								ObjType::DYNAMO, { 12, 0, 0, 0 },
								ObjType::REFORMER, { 6, 0, 0, 0 },
							};
		
		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

