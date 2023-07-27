/*
	mission_haunted.cpp
	Ghosts and landers mission in Defence Condition.

	Copyright 2010 Daylon Graphics Ltd.
*/




#include "MilitaryMission.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


#include "Common/util_color.h"
#include "Common/util_geom.h"


#include "Globals/prefs.h"

#include "GameObjects/player.h"
#include "GameObjects/obj_types.h"

#include "Arenas/DefconPlayViewBase.h"




void Defcon::CHaunted::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 81;
	m_nLandersRemaining  = 30;
}


bool Defcon::CHaunted::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CHaunted::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"Landers are arriving with a ghost escort.",
		"Engage enemy forces and eliminate."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
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

		size_t i, j;
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
}

