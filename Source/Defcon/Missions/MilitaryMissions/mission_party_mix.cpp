// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_random.cpp
	"Party Mix" mission in Defence Condition.
	The enemy types are chosen totally at random.

	Copyright 2004 Daylon Graphics Ltd.
*/


#include "MilitaryMission.h"




void Defcon::CPartyMixMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 15 + 20 + 25 + 20;

	// Precompute the enemy types so we know how many landers we'll have.

	for(size_t i = 0; i < array_size(Waves); i++)
	{
		for(int32 AttackWave = 0; AttackWave < 4; AttackWave++)
		{
			for(size_t j = 0; j < Waves[i].count[AttackWave]; j++)
			{
				const auto Kind = EnemyTypes[IRAND(array_size(EnemyTypes))];

				if(Kind == ObjType::LANDER)
				{
					m_nLandersRemaining++;
				}
				ChosenEnemyTypes.Add(Kind);
			}
		}
	}
}


bool Defcon::CPartyMixMission::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CPartyMixMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"The Apex are temporarily thrown into chaos",
		"and can only mount a mixed, random counterattack.",
		"There's no telling what will come your way."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CPartyMixMission::MakeTargets(float fElapsed, const CFPoint& where)
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

		if(m_nAttackWave >= 4)
			return;


		size_t i, j;

		for(i = 0; i < array_size(Waves); i++)
		{
			for(j = 0; j < Waves[i].count[m_nAttackWave] && this->HostilesRemaining() > 0; j++)
			{
				float wp = m_pArena->GetWidth();
				float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
				x = (float)fmod(x, wp);
				float y = (FRAND * .75f + .15f) * m_pArena->GetHeight();

				m_pArena->CreateEnemy(ChosenEnemyTypes[IdxEnemyTypes++], CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * j), true, true);
			}
		}

		m_nAttackWave++;
	}
}
