// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firstcontact.cpp
	"First Contact" mission in Defence Condition.
	The easiest military engagement;	just a few landers.
*/


#include "MilitaryMission.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




void Defcon::CFirstContactMission::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	//m_nHostilesRemaining = 9;
	//m_nLandersRemaining  = 9;

	IntroText =
		"A small group of Landers is approaching.\n"
		"\n"
		"Engage enemy forces and eliminate them."
	;

	AddEnemySpawnInfo({ObjType::LANDER, { 4, 3, 2, 0 }});
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

	UpdateWaves(where);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
