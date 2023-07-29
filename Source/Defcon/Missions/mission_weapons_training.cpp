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



void Defcon::CWeaponsTrainingMission::Init(UDefconPlayViewBase* pA)
{
	IMission::Init(pA);

	m_bTargetsMade = false;

	NumTargetsHit = 0;
}


bool Defcon::CWeaponsTrainingMission::Update(float fElapsed)
{
	if(!IMission::Update(fElapsed))
	{
		return false;
	}

	if(m_fAge < 5.0f)
	{
		this->DoIntroText(fElapsed);
	}
	else if(m_fAge < 6.0f)
	{
		this->DoMakeTargets(fElapsed);
	}
	else if(this->AreAllTargetsHit(fElapsed))
	{
		// We're done.
		return false;
	}

	return true;
}


void Defcon::CWeaponsTrainingMission::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
	{
		return;
	}

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"Now we'll practice shooting.",
		"",
		"Destroy the targets with your ship's laser cannon.",
		"Pressing the spacebar will fire a round.",
		"",
		"When you have destroyed all the targets,",
		"you'll be cleared for your first real mission."
	};


	for(auto p : psz)
	{
		m_pArena->AddMessage(FString(p));
	}
}


void Defcon::CWeaponsTrainingMission::DoMakeTargets(float fElapsed)
{
	if(m_bTargetsMade)
	{
		return;
	}

	m_bTargetsMade = true;

	for(size_t i = 0; i < NumTargets; i++)
	{
		CBeacon* p = new CBeacon;
		p->InstallSprite();

		p->m_fLifespan = 2.0f;
		p->MakeHurtable();
		p->m_pos.set(
			MAP(i, 0, 6, m_pArena->GetDisplayWidth()*.66f, m_pArena->GetWidth() * 0.9f),
			SFRAND * 0.33f * m_pArena->GetHeight() + m_pArena->GetHeight()/2);

		//p->SetAsMissionTarget();
		m_pArena->GetObjects().Add(p);
	}
}



bool Defcon::CWeaponsTrainingMission::AreAllTargetsHit(float fElapsed)
{
	const int32 NumTargetsLeft = m_pArena->GetObjects().CountOf(ObjType::BEACON);
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

		m_pArena->AddMessage(Str);
	}

	return (m_pArena->GetObjects().Find(ObjType::BEACON, nullptr) == nullptr);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE