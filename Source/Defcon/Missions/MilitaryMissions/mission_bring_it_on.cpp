/*
	mission_bring_it_on.cpp.cpp
	"Bring it On" mission in Defence Condition.
	This calls for destroying several dynamos 
	and a few reformers while baiters arrive early.

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
#if 0
		size_t i;
		for(i = 0; i < numDynamos[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			//CCreateEnemyEvent* p = new CCreateEnemyEvent;
			p->Init(m_pArena);
			p->m_objtype = ObjType::DYNAMO;
			p->m_when = UKismetSystemLibrary::GetGameTimeInSeconds(m_pArena) + (FRAND * 0.5f * i);
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .75f + .15f) * m_pArena->GetHeight();
			p->m_where.set(x, y);
			this->AddEvent(p);
		}

		for(i = 0; i < numReformers[m_nAttackWave] && this->HostilesRemaining() > 0; i++)
		{
			//CCreateEnemyEvent* p = new CCreateEnemyEvent;
			p->Init(m_pArena);
			p->m_objtype = ObjType::REFORMER;
			p->m_when = UKismetSystemLibrary::GetGameTimeInSeconds(m_pArena) + (FRAND * 0.5f * i);
			float wp = m_pArena->GetWidth();
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;
			x = (float)fmod(x, wp);
			float y = (FRAND * .75f + .15f) * m_pArena->GetHeight();
			p->m_where.set(x, y);
			this->AddEvent(p);
		}
#endif
		m_nAttackWave++;
	}
}

