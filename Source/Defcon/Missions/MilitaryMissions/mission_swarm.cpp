// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_swarm.cpp
	"Swarm" mission in Defence Condition.
*/


#include "MilitaryMission.h"


// ----------------------------------------------------------

#if 0
void Defcon::CSwarm::Init()
{
	Super::Init();

	NumTargetsRemaining = 30 + 45 + 80 + 10;
}


bool Defcon::CSwarm::Update(float fElapsed)
{
	if(!Super::Update(fElapsed))
		return false;

	DoIntroText(fElapsed);

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
	if(TargetsRemaining() > 0 
		&& Age >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		AddMissionCleaner(where);
	}


	if((TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (TotalHostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		if(WaveIndex >= 3)
			return;

		const Wave waves[] = 
		{
			{ EObjType::SWARMER, { 30, 45, 80 } },
			{ EObjType::GHOST, { 3, 4, 3 } },
		};


		int32 i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[WaveIndex]; j++)
			{
				CCreateEnemyTask* p = new CCreateEnemyTask;
				p->Init(gpArena);
				p->EnemyType = waves[i].type;
				p->Countdown = FRAND * 0.1f * j;
				float wp = gpArena->GetWidth();
				float x = (FRAND - 0.5f) * 0.2f * gpArena->GetDisplayWidth() + wp/2;
				x = (float)fmod(x, wp);
				float y = (float)gpArena->GetHeight() * FRAND;
				p->Where.Set(x, y);
				AddTask(p);
			}
		}

		WaveIndex++;
	}
}

#endif
