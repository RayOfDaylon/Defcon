// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_pack.cpp
	A small, dense group of firebombers on the opposite 
	side of the planet.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberPack::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 40;
}


bool Defcon::CFirebomberPack::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CFirebomberPack::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"A rogue group of firebombers has tightly",
		"clustered on the far side of the planet.",
		"",
		"Their combined firepower in tight quarters",
		"may require deploying smartbombs."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CFirebomberPack::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((    this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0  && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		const size_t numBombers[] = { 20, 5 }; 
		const size_t numEscorts[] = { 10, 5 }; 

		if(m_nAttackWave >= array_size(numBombers))
		{
			return;
		}

		/*const Wave waves[] =
		{
			{ ObjType::FIREBOMBER_TRUE, { 10, 3, 0, 0 } },
			{ ObjType::FIREBOMBER_WEAK, { 10, 3, 0, 0 } },
			{ ObjType::DYNAMO,          { 10, 5, 0, 0 } }
		};*/


		size_t i;

		for(i = 0; i < numBombers[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * m_pArena->GetDisplayWidth() + wp/2;
			x = (float)fmod(x, wp);
			float y = FRANDRANGE(0.25f, 0.85f) * m_pArena->GetHeight();

			m_pArena->CreateEnemy((i & 1) == 1 ? ObjType::FIREBOMBER_TRUE : ObjType::FIREBOMBER_WEAK, 
					CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}

		for(i = 0; i < numEscorts[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * m_pArena->GetDisplayWidth() + wp / 2;
			x = (float)fmod(x, wp);
			float y = FRANDRANGE(0.25f, 0.85f) * m_pArena->GetHeight();

			m_pArena->CreateEnemy(ObjType::DYNAMO, CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}
		m_nAttackWave++;
	}
}
