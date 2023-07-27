/*
	mission_ghost.cpp
	"Ghost" mission in Defence Condition.

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


void Defcon::CGhostMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 9 + 40 + 6;
}


bool Defcon::CGhostMission::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CGhostMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"The 'ghost' reformer variant has appeared en masse.",
		"They are based on some type of controlled plasma",
		"and can break up and reform at will. They are",
		"impossible to destroy at close range.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CGhostMission::MakeTargets(float fElapsed, const CFPoint& where)
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

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] = 
		{
			{ ObjType::DYNAMO, { 2, 5, 2 } },
			{ ObjType::GHOST, { 10, 15, 15 } },
			{ ObjType::REFORMER, { 3, 2, 1 } }
		};

		SPAWN_ENEMIES(0.1f, 0.1f, 0.85f)

		m_nAttackWave++;
	}
}

