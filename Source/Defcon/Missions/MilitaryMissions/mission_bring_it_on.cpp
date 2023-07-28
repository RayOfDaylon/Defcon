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
}


bool Defcon::CBringItOn::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
	{
		return false;
	}

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CBringItOn::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.

	const char* psz[] = 
	{
		"Reformers and dynamos have laid a trap.",
		"They have signalled baiters, phreds and munchies",
		"to arrive early in a high-numbered ambush.",
		"",
		"Don't let the ambush distract you from",
		"properly eliminating the ordinary enemies,",
		"otherwise you will be overwhelmed.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
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
		//const size_t numDynamos[] = { 12 }; 
		//const size_t numReformers[] = { 6 }; 
		
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

