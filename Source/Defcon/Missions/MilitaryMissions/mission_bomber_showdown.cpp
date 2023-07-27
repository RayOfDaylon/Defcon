/*
	mission_bomber_showdown.cpp
	"Bomber showdown" mission in Defence Condition.

	Copyright 2004 Daylon Graphics Ltd.
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


void Defcon::CBomberShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 18+6;
	// 9*2 bombers + 3*2 pods
}


bool Defcon::CBomberShowdown::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
	{
		return false;
	}

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CBomberShowdown::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.

	const char* psz[] = 
	{
		"An armada of bombers has been detected.",
		"They will fill the sky with mines; ",
		"avoid impacting them at all costs.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CBomberShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 2)
		{
			return;
		}

		const Wave waves[] = 
		{
			{ ObjType::BOMBER, { 9, 9, 0, 0 } },
			{ ObjType::POD,    { 3, 3, 0, 0 } }
		};


		STANDARD_ENEMY_SPAWNING(0.5f)


#if 0
		const size_t numBombers[] = { 9, 9 }; 
		const size_t numEscorts[] = { 3, 3 }; 

		if(m_nAttackWave >= 2/*array_size(numBombers)*/)
			return;

		size_t i;
		for(i = 0; i < numBombers[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			//CCreateEnemyEvent* p = new CCreateEnemyEvent;
			p->Init(m_pArena);
			p->m_objtype = ObjType::BOMBER;
			p->m_when = UKismetSystemLibrary::GetGameTimeInSeconds(m_pArena) + (FRAND * 0.5f * i);
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();
			p->m_where.set(x, y);
			this->AddEvent(p);
		}

		for(i = 0; i < numEscorts[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			//CCreateEnemyEvent* p = new CCreateEnemyEvent;
			p->Init(m_pArena);
			p->m_objtype = ObjType::POD;
			p->m_when = UKismetSystemLibrary::GetGameTimeInSeconds(m_pArena) + (FRAND * 0.5f * i);
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .15f + .85f) * m_pArena->GetHeight();
			p->m_where.set(x, y);
			this->AddEvent(p);
		}
#endif
		m_nAttackWave++;
	}
}

