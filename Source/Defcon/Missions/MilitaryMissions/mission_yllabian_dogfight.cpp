// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_dogfight.cpp
	"Yllabian Dogfight" mission in Defence Condition.

	Copyright 2004 Daylon Graphics Ltd.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianDogfight::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 12+15+9+6+3+6;

	IntroText =
		"Yllabian space guppies and swarmers have set an ambush.\n"
		"\n"
		"Engage enemy forces and eliminate them."
		;
}


void Defcon::CYllabianDogfight::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{
		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 3)
			return;

		const FEnemySpawnCounts waves[] = 
		{
			{ ObjType::GUPPY,   { 12, 15, 9, 0 } },
			{ ObjType::SWARMER, {  6,  3, 6, 0 } }
		};


		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].NumPerWave[m_nAttackWave] && this->HostilesRemaining() > 0; j++)
			{
				float wp = m_pArena->GetWidth();
				float x = (FRAND - 0.5f) * m_pArena->GetDisplayWidth() + wp/2;
				x = (float)fmod(x, wp);
				float y = FRANDRANGE(0.25f, 0.75f) * m_pArena->GetHeight();

				m_pArena->CreateEnemy(waves[i].Kind, CFPoint(x, y), FRANDRANGE(0.0f, 0.1f * j), true, true);
			}
		}

		m_nAttackWave++;
	}
}

