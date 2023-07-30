// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_swarm.cpp
	"Swarm" mission in Defence Condition.

	Copyright 2004 Daylon Graphics Ltd.
*/


#include "MilitaryMission.h"


// ----------------------------------------------------------

#if 0
void Defcon::CSwarm::Init()
{
	CMilitaryMission::Init();

	NumHostilesRemaining = 30 + 45 + 80 + 10;
}


bool Defcon::CSwarm::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CSwarm::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(IntroIsDone)
		return;

	IntroIsDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"A huge swarmer fleet has decided",
		"to destroy you all by itself.",
		"",
		"Engage enemy forces and eliminate them.",
	};

	for(auto Text : psz)
	{
		gpArena->AddMessage(Text);
	}
}


void Defcon::CSwarm::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		this->AddMissionCleaner(where);
	}


	if((this->HostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		if(WaveIndex >= 3)
			return;

		const Wave waves[] = 
		{
			{ ObjType::SWARMER, { 30, 45, 80 } },
			{ ObjType::GHOST, { 3, 4, 3 } },
		};


		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[WaveIndex] && this->HostilesRemaining() > 0; j++)
			{
				CCreateEnemyEvent* p = new CCreateEnemyEvent;
				p->Init(gpArena);
				p->m_objtype = waves[i].type;
				p->m_when = GameTime() + FRAND * 0.1f * j;
				float wp = gpArena->GetWidth();
				float x = (FRAND - 0.5f) * 0.2f * gpArena->GetDisplayWidth() + wp/2;
				x = (float)fmod(x, wp);
				float y = (float)gpArena->GetHeight() * FRAND;
				p->m_where.Set(x, y);
				this->AddEvent(p);
			}
		}

		WaveIndex++;
	}
}

#endif
