/*
	mission_firstcontact.cpp
	"First Contact" mission in Defence Condition.
	The easiest actual military engagement;
	just a few landers.

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

#pragma optimize("", off)


// ----------------------------------------------------------


void Defcon::CFirstContactMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 9;
	m_nLandersRemaining  = 9;
}


bool Defcon::CFirstContactMission::Update(float fElapsed)
{
	// - Show some intro text.
	// - Make nine landers appear, first wave
	// - of four, second of three, third of two.
	//   Make them come in every DELAY_BETWEEN_REATTACK 
	//   or if no hostiles exist.
	// - When all hostiles gone,
	//   the mission ends.

	if(!CMilitaryMission::Update(fElapsed))
	{
		return false;
	}

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CFirstContactMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"A small group of Landers is approaching.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CFirstContactMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 && m_fAge >= 25.0f)
	{
		// Add baiters until player clears minimal hostiles.
		if(m_fAge - m_fLastCleaner >= 2.0f)
		{
			this->AddBaiter(where);
		}
	}

	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{

		m_fRepopCounter = 0.0f;

		const Wave waves[] = { ObjType::LANDER, { 4, 3, 2, 0 } };

		//const size_t numLanders[] = { 4, 3, 2 }; 
		if(m_nAttackWave >= 3/*array_size(numLanders)*/)
		{
			return;
		}

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}


#pragma optimize("", on)
