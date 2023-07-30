// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission.cpp
	IMission base class
	Defines commonalities of missions in Defence Condition.
*/


#include "mission.h"

#include "DefconUtils.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/player.h"
#include "GameObjects/human.h"
#include "GameObjects/Auxiliary/stargate.h"
#include "GameObjects/Enemies/lander.h"
#include "GameObjects/Enemies/guppy.h"
#include "GameObjects/Enemies/hunter.h"
#include "GameObjects/Enemies/bomber.h"
#include "GameObjects/Enemies/pod.h"
#include "GameObjects/Enemies/swarmer.h"
#include "GameObjects/Enemies/baiter.h"
#include "GameObjects/Enemies/firebomber.h"
#include "GameObjects/Enemies/fireball.h"
#include "GameObjects/Enemies/dynamo.h"
#include "GameObjects/Enemies/spacehum.h"
#include "GameObjects/Enemies/reformer.h"
#include "GameObjects/Enemies/bouncer.h"
#include "GameObjects/Enemies/munchies.h"
#include "GameObjects/Enemies/ghost.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




void Defcon::IMission::Init(UDefconPlayViewBase* p)
{
	m_pArena = p; 

	this->CreateTerrain();
	this->AddHumanoids();

	m_pArena->GetPlayerShip().EnableInput(false);
}


bool Defcon::IMission::Update(float DeltaTime) 
{
	m_fAge += DeltaTime; 

	m_events.Process();

	DoIntroText(DeltaTime);

	return true; 
}


void Defcon::IMission::DoIntroText(float DeltaTime)
{
	if(!m_bIntroDone)
	{
		m_bIntroDone = true;

		TArray<FString> IntroTextLines;
		IntroText.ParseIntoArray(IntroTextLines, TEXT("\n"), false);

		for(const auto& IntroTextLine : IntroTextLines)
		{
			m_pArena->AddMessage(IntroTextLine);
		}
	}
}


void Defcon::IMission::CreateTerrain()
{
	m_pArena->CreateTerrain();
}


void Defcon::IMission::AddHumanoids()
{
	// Most missions have humanoids. 
	// We mod the game's humanoids for the arena.

	auto& Humans = m_pArena->GetHumans();

	Humans.ForEach([&](Defcon::IGameObject* Obj)
	{
		auto Human = static_cast<CHuman*>(Obj);
		Human->m_pObjects  = &m_pArena->GetObjects();
		Human->m_pObjects2 = &m_pArena->GetEnemies();

		// If we're on wave 2 or higher, don't move the humans
		// except to reset them vertically.
		Human->InitHuman(CFPoint(
			(gDefconGameInstance->GetScore() == 0) ? FRAND * m_pArena->GetWidth() * HUMAN_DISTRIBUTION : Human->m_pos.x, 
			FRAND * 5 + 25));
	}
	);
}


void Defcon::IMission::Conclude()
{
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
