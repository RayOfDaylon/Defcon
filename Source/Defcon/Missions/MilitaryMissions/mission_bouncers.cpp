/*
	mission_bouncers.cpp
	Bouncers, landers, firebombers, and bombers.

	Copyright 2009-2010 Daylon Graphics Ltd.
*/


#include "MilitaryMission.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


#include "Common/util_color.h"
#include "Common/util_geom.h"


#include "Globals/prefs.h"

#include "GameObjects/player.h"
#include "GameObjects/obj_types.h"


#include "Arenas/DefconPlayViewBase.h"



// ----------------------------------------------------------


void Defcon::CBouncersMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 71 + 9 + 9;
	m_nLandersRemaining  = 30;
}


bool Defcon::CBouncersMission::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CBouncersMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"A group of bouncers has decided",
		"to 'drop' in."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CBouncersMission::MakeTargets(float fElapsed, const CFPoint& where)
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
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] = 
		{
			{ ObjType::LANDER, { 10, 10, 10, } },
			{ ObjType::BOMBER, { 3, 5, 3 } },
			{ ObjType::BOUNCER_TRUE, { 5, 5, 5 } }, 
			{ ObjType::BOUNCER_WEAK, { 3, 3, 3 } }, 
			{ ObjType::FIREBOMBER_TRUE, { 5, 5, 5 } },
			{ ObjType::FIREBOMBER_WEAK, { 3, 3, 3 } }
		};

		STANDARD_ENEMY_SPAWNING(0.1f)

#if 0
		size_t i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[m_nAttackWave]; j++)
			{
				//CCreateEnemyEvent* p = new CCreateEnemyEvent;
				p->Init(m_pArena);
				p->m_objtype = waves[i].type;
				p->m_when = UKismetSystemLibrary::GetGameTimeInSeconds(m_pArena) + (FRAND * 0.1f * j);
				float wp = m_pArena->GetWidth();
				float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
				x = (float)fmod(x, wp);
				float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();
				p->m_where.set(x, y);
				this->AddEvent(p);
			}
		}
#endif
		m_nAttackWave++;
	}
}

