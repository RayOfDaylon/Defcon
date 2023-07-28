// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_showdown.cpp
	"Firebomber showdown" mission in Defence Condition.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 12 + 6;
	// 6*2 firebombers + 6 dynamos
}


bool Defcon::CFirebomberShowdown::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CFirebomberShowdown::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"Firebombers have moved in to engage you.",
		"Try to fire at them from a distance.",
		"Also watch out for dynamo escorts.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CFirebomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;
		const size_t numBombers[] = { 6, 6 }; 
		const size_t numEscorts[] = { 3, 3 }; 

		if(m_nAttackWave >= array_size(numBombers))
			return;

		size_t i;
		for(i = 0; i < numBombers[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();

			m_pArena->CreateEnemy((i & 1) == 1 ? ObjType::FIREBOMBER_TRUE : ObjType::FIREBOMBER_WEAK, 
				CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}

		for(i = 0; i < numEscorts[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();

			m_pArena->CreateEnemy(ObjType::DYNAMO, CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}
		m_nAttackWave++;
	}
}

