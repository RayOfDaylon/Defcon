// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_yllabian_escort.cpp
	"Yllabian Dogfight 2" but in orbit.
*/



#include "MilitaryMission.h"



void Defcon::CYllabianEscort::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	/*m_nHostilesRemaining = 24+30+30+30
							+3+3+3+3
							+3+3+3
							+10+7+4+3
							+3+4+4+3
							+10+12+10+5;

	m_nLandersRemaining =  10 + 12 + 10 + 5;*/

	IntroText = 
		"Yllabian fighters are escorting a lander attack.\n"
		"Engage enemy forces and eliminate them."
	;

	JFactor = 0.1f;

	AddEnemySpawnInfo({ ObjType::GUPPY,   { 24, 30, 30, 30 } });
	AddEnemySpawnInfo({ ObjType::SWARMER, {  3,  3,  3,  3 } });
	AddEnemySpawnInfo({ ObjType::POD,     {  3,  3,  3,  0 } });
	AddEnemySpawnInfo({ ObjType::DYNAMO,  { 10,  7,  4,  3 } });
	AddEnemySpawnInfo({ ObjType::HUNTER,  {  3,  4,  4,  3 } });
	AddEnemySpawnInfo({ ObjType::LANDER,  { 10, 12, 10,  5 } });
}


void Defcon::CYllabianEscort::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		this->AddMissionCleaner(where);
	}

	UpdateWaves(where);

#if 0
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{
		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 4)
			return;

		const Wave waves[] = 
		{
			{ ObjType::GUPPY,   { 24, 30, 30, 30 } },
			{ ObjType::SWARMER, {  3,  3,  3,  3 } },
			{ ObjType::POD,     {  3,  3,  3,  0 } },
			{ ObjType::DYNAMO,  { 10,  7,  4,  3 } },
			{ ObjType::HUNTER,  {  3,  4,  4,  3 } },
			{ ObjType::LANDER,  { 10, 12, 10,  5 } }
		};


		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[m_nAttackWave] && this->HostilesRemaining() > 0; j++)
			{
				float wp = m_pArena->GetWidth();
				CFPoint P;

				switch(waves[i].type)
				{
					case ObjType::LANDER:
						P.x = FRAND * (m_pArena->GetWidth() - 1);
						P.y = FRANDRANGE(0.85f, 1.0f);
						break;

					case ObjType::POD:
					case ObjType::HUNTER:
					case ObjType::DYNAMO:
						P.x = FRAND * (m_pArena->GetWidth() - 1);
						P.y = FRANDRANGE(0.15f, 0.85f);
						break;

					default:
						P.x = (FRAND - 0.5f) * m_pArena->GetDisplayWidth() + wp/2;
						P.y = FRANDRANGE(0.25f, 0.75f);
						break;
				}
				P.x = (float)fmod(P.x, wp);
				P.y *= m_pArena->GetHeight();

				m_pArena->CreateEnemy(waves[i].type, P, FRANDRANGE(0.0f, 0.1f * j), true, true);
			}
		}

		m_nAttackWave++;
	}
#endif
}
