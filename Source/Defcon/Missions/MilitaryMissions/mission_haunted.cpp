// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_haunted.cpp
	Ghosts and landers mission in Defence Condition.
*/


#include "MilitaryMission.h"


void Defcon::CHaunted::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	//m_nHostilesRemaining = 81;
	//m_nLandersRemaining  = 30;

	IntroText =
		"Landers are arriving with a ghost escort.\n"
		"Engage enemy forces and eliminate."
		;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ ObjType::LANDER, { 10, 10, 10 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO, {  3,  5,  3 } });
	AddEnemySpawnInfo({ ObjType::GHOST,  { 10, 15, 15 } });
}


void Defcon::CHaunted::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
	{
		this->AddMissionCleaner(where);
	}

	UpdateWaves(where);

#if 0
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| m_fRepopCounter > DELAY_BETWEEN_REATTACK)
	{
		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] = 
		{
			{ ObjType::LANDER, { 10, 10, 10, } },
			{ ObjType::DYNAMO, { 3, 5, 3 } },
			{ ObjType::GHOST,  { 10, 15, 15 } }
		};

		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[m_nAttackWave]; j++)
			{
				float wp = m_pArena->GetWidth();
				float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
				x = (float)fmod(x, wp);
				float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();

				if(waves[i].type != ObjType::LANDER)
				{
					y = FRANDRANGE(0.15f, 0.85f) * m_pArena->GetHeight();
				}

				m_pArena->CreateEnemy(waves[i].type, CFPoint(x, y), FRANDRANGE(0.0f, 0.1f * j), true, true);
			}
		}

		m_nAttackWave++;
	}
#endif
}

