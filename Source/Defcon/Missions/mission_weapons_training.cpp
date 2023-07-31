// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_weapons_training.cpp
	Defines weapons training mission in Defence Condition.
*/


#include "mission.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/player.h"
#include "GameObjects/beacon.h"
#include "Arenas/DefconPlayViewBase.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



constexpr int32 NumTargets = 15;



void Defcon::CWeaponsTrainingMission::Init()
{
	IMission::Init();

	TargetsMade = false;

	NumTargetsHit = 0;

	IntroText = 
		"Now we'll practice shooting.\n"
		"\n"
		"Destroy the targets with your ship's laser cannon.\n"
		"Pressing the spacebar will fire a round.\n"
		"\n"
		"When you have destroyed all the targets,\n"
		"you'll be cleared for your first real mission."
		;

}


bool Defcon::CWeaponsTrainingMission::Update(float fElapsed)
{
	if(!IMission::Update(fElapsed))
	{
		return false;
	}

	if(!TargetsMade)
	{
		this->DoMakeTargets(fElapsed);
		TargetsMade = true;
	}

	if(this->AreAllTargetsHit(fElapsed))
	{
		// We're done.
		return false;
	}

	return true;
}


void Defcon::CWeaponsTrainingMission::DoMakeTargets(float fElapsed)
{
	for(int32 i = 0; i < NumTargets; i++)
	{
		CBeacon* p = new CBeacon;
		p->InstallSprite();

		p->Lifespan = 2.0f;
		p->MakeHurtable();
		p->Position.Set(
			MAP(i, 0, 6, gpArena->GetDisplayWidth()*.66f, gpArena->GetWidth() * 0.9f),
			SFRAND * 0.33f * gpArena->GetHeight() + gpArena->GetHeight()/2);

		gpArena->GetObjects().Add(p);
	}
}



bool Defcon::CWeaponsTrainingMission::AreAllTargetsHit(float fElapsed)
{
	const int32 NumTargetsLeft = gpArena->GetObjects().CountOf(EObjType::BEACON);
	const int32 NumHit         = NumTargets - NumTargetsLeft;

	if(NumHit != NumTargetsHit)
	{
		NumTargetsHit = NumHit;

		FString Str;

		if(NumTargetsLeft > 0)
		{
			Str = FString::Printf(TEXT("%d target%s destroyed, %d to go"), NumTargetsHit, NumTargetsHit == 1 ? TEXT("") : TEXT("s"), NumTargetsLeft);
		}
		else
		{
			Str = TEXT("All targets destroyed.");
		}

		gpArena->AddMessage(Str);
	}

	return (gpArena->GetObjects().Find(EObjType::BEACON) == nullptr);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
