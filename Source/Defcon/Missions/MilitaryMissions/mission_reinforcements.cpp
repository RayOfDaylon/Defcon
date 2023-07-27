/*
	mission_reinforcements.cpp
	"Reinforcements" mission in Defence Condition.
	The landers are escorted by dynamos.

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


void Defcon::CReinforcedMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 12 + 6;
	m_nLandersRemaining  = 12;
	// 3*4 landers + 6 dynamos
}


bool Defcon::CReinforcedMission::Update(float fElapsed)
{
	// - Show some intro text.
	// - Make nine landers appear, first wave
	// - of four, second of three, third of two.
	//   Make them come in every DELAY_BETWEEN_REATTACK 
	//   or if no hostiles exist.
	// - When all hostiles gone,
	//   the mission ends.

	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CReinforcedMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.

	const char* psz[] = 
	{
		"More landers are coming.",
		"This time, they have dynamo escorts.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CReinforcedMission::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3
			)
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

		if(m_nAttackWave >= 3)
			return;

		const Wave waves[] =
		{
			{ ObjType::LANDER, { 4, 4, 4, 0 } },
			{ ObjType::DYNAMO, { 3, 2, 1, 0 } }
		};

		STANDARD_ENEMY_SPAWNING(0.5f)

		m_nAttackWave++;
	}
}

