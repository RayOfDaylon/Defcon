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

	MaxWaves = 2;

	NumTargetsRemaining = 40;

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
	if(WaveIndex >= MaxWaves)
	{
		return;
	}

	if((    TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		|| (TotalHostilesInPlay() > 0  && RepopCounter > DELAY_BETWEEN_REATTACK))
	{
		RepopCounter = 0.0f;

		const int32 numBombers[] = { 20, 5 }; 
		const int32 numEscorts[] = { 10, 5 }; 

		/*const Wave waves[] =
		{
			{ EObjType::FIREBOMBER_TRUE, { 10, 3, 0, 0 } },
			{ EObjType::FIREBOMBER_WEAK, { 10, 3, 0, 0 } },
			{ EObjType::DYNAMO,          { 10, 5, 0, 0 } }
		};*/


		const float ArenaWidth = GArena->GetWidth();

		int32 i;

		for(i = 0; i < numBombers[WaveIndex]; i++)
		{
			float x = (FRAND - 0.5f) * GArena->GetDisplayWidth() + ArenaWidth/2;
			x = (float)fmod(x, ArenaWidth);
			float y = FRANDRANGE(0.25f, 0.85f) * GArena->GetHeight();

			GArena->CreateEnemy((i & 1) == 1 ? EObjType::FIREBOMBER_TRUE : EObjType::FIREBOMBER_WEAK, EObjType::UNKNOWN,
					CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), EObjectCreationFlags::StandardEnemy);
		}

		for(i = 0; i < numEscorts[WaveIndex]; i++)
		{
			float wp = GArena->GetWidth();
			float x = (FRAND - 0.5f) * GArena->GetDisplayWidth() + wp / 2;
			x = (float)fmod(x, wp);
			float y = FRANDRANGE(0.25f, 0.85f) * GArena->GetHeight();

			GArena->CreateEnemy(EObjType::DYNAMO, EObjType::UNKNOWN, CFPoint(x, y), FRANDRANGE(0.0f, 0.5f * i), EObjectCreationFlags::StandardEnemy);
		}
		WaveIndex++;
	}
}

