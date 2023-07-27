/*
	mission_reformer_showdown.cpp
	"Reformer showdown" mission in Defence Condition.

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


void Defcon::CReformerShowdown::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 6+6;
}


bool Defcon::CReformerShowdown::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CReformerShowdown::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"Reformers have moved in to engage you.",
		"Destroy their parts before they regroup.",
		"Also watch out for swarmer escorts.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CReformerShowdown::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 2)
			return;

		const Wave waves[] =
		{
			{ ObjType::REFORMER, { 4, 2, 0, 0 } },
			{ ObjType::SWARMER,  { 3, 3, 0, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

