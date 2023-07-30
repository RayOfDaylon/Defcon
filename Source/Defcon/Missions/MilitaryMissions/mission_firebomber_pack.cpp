// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_firebomber_pack.cpp
	A small, dense group of firebombers on the opposite 
	side of the planet.
*/


#include "MilitaryMission.h"



void Defcon::CFirebomberPack::Init()
{
	CMilitaryMission::Init();

	NumHostilesRemaining = 40;

	IntroText = 
		"A rogue group of firebombers has tightly\n"
		"clustered on the far side of the planet.\n"
		"\n"
		"Their combined firepower in tight quarters\n"
		"may require deploying smartbombs.\n"
	;
}


void Defcon::CFirebomberPack::MakeTargets(float fElapsed, const CFPoint& where)
{
	if((    this->HostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0  && RepopCounter > DELAY_BETWEEN_REATTACK))
	{

		RepopCounter = 0.0f;

		const int32 numBombers[] = { 20, 5 }; 
		const int32 numEscorts[] = { 10, 5 }; 

		if(WaveIndex >= array_size(numBombers))
		{
			return;
		}

		/*const Wave waves[] =
		{
			{ ObjType::FIREBOMBER_TRUE, { 10, 3, 0, 0 } },
			{ ObjType::FIREBOMBER_WEAK, { 10, 3, 0, 0 } },
			{ ObjType::DYNAMO,          { 10, 5, 0, 0 } }
		};*/


		int32 i;

		for(i = 0; i < numBombers[WaveIndex] && this->HostilesRemaining() > 0; i++)
		{
			float wp = gpArena->GetWidth();
			float x = (FRAND - 0.5f) * gpArena->GetDisplayWidth() + wp/2;
			x = (float)fmod(x, wp);
			float y = FRANDRANGE(0.25f, 0.85f) * gpArena->GetHeight();

			gpArena->CreateEnemy((i & 1) == 1 ? ObjType::FIREBOMBER_TRUE : ObjType::FIREBOMBER_WEAK, 
					CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}

		for(i = 0; i < numEscorts[WaveIndex] && this->HostilesRemaining() > 0; i++)
		{
			float wp = gpArena->GetWidth();
			float x = (FRAND - 0.5f) * gpArena->GetDisplayWidth() + wp / 2;
			x = (float)fmod(x, wp);
			float y = FRANDRANGE(0.25f, 0.85f) * gpArena->GetHeight();

			gpArena->CreateEnemy(ObjType::DYNAMO, CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), true, true);
		}
		WaveIndex++;
	}
}

