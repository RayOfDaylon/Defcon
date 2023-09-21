// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	task.cpp
	Defcon scheduled task system.
	Scheduled tasks are used to easily run code that needs to occur later, 
	e.g. enemy spawns, arena closures, etc.
*/

#include "task.h"

#include "Arenas/DefconPlayViewBase.h"
#include "Main/GameMatch.h"
#include "DefconGameInstance.h"
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
#include "GameObjects/Enemies/turret.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



void Defcon::CRestartMissionTask::Do()
{
	if(GGameMatch->GetHumans().Count() == 0)
	{
		// The player ship blew up but there are no more humans to defend, so...
		GDefconGameInstance->TransitionToArena(EDefconArena::GameOver);
		return;
	}

	GGameMatch->SetCurrentMission(GGameMatch->GetMission()->GetID());
	GDefconGameInstance->TransitionToArena(EDefconArena::Prewave);
}

// -----------------------------------------------------

void Defcon::CEndMissionTask::Do()
{
	// todo: surely we can do better than deleting another object's member and setting other object's object's pointers.

	SAFE_DELETE(GArena->Terrain);

	GArena->PlayAreaMain ->SetTerrain(nullptr);
	GArena->PlayAreaRadar->SetTerrain(nullptr);

	Defcon::GGameMatch->MissionEnded();
}

// --------------------------------------------------------

void Defcon::CCreateGameObjectTask::Do()
{
	CEnemy* GameObj = InstantiateGameObject();

	check(GameObj != nullptr);

	static int32 ObjectID = 0;

	GameObj->SetID(ObjectID++);

	GameObj->SetCreatorType(CreatorType);

	GameObj->MapperPtr = &GArena->GetMainAreaMapper();
	GameObj->Position  = Where;
	GameObj->Orientation.Fwd.x = SBRAND;

	GameObj->Init(CFPoint(GArena->GetWidth(),        GArena->GetHeight()), 
                  CFPoint(GArena->GetDisplayWidth(), GArena->GetHeight()));

	GameObj->SetTarget(&GArena->GetPlayerShip());
	GameObj->MakeHurtable();
	GameObj->SetCollisionInjurious();

	GameObj->InstallSprite();
	GameObj->SetAsMissionTarget(bMissionTarget);

	// Let the specific game object add or override any of its settings.
	GameObj->OnFinishedCreating(Options);

	GArena->GetEnemies().Add(GameObj);
}


Defcon::CEnemy* Defcon::CCreateGameObjectTask::InstantiateGameObject()
{
	CEnemy* GameObj = nullptr;

	switch(ObjType)
	{
		case EObjType::BAITER:          GameObj = new CBaiter;           break;
		case EObjType::BIGRED:          GameObj = new CBigRed;           break;
		case EObjType::BOMBER:          GameObj = new CBomber;           break;
		case EObjType::BOUNCER_TRUE:    GameObj = new CBouncer;          break;
		case EObjType::BOUNCER_WEAK:    GameObj = new CWeakBouncer;      break;
		case EObjType::DYNAMO:          GameObj = new CDynamo;           break;
		case EObjType::FIREBALL:        GameObj = new CFireball;         break;
		case EObjType::FIREBOMBER_TRUE: GameObj = new CFirebomber;       break;
		case EObjType::FIREBOMBER_WEAK: GameObj = new CWeakFirebomber;   break;
		case EObjType::GHOST:           GameObj = new CGhost;            break;
		case EObjType::GHOSTPART:       GameObj = new CGhostPart;        break;
		case EObjType::GUPPY:           GameObj = new CGuppy;            break;
		case EObjType::LANDER:          GameObj = new CLander;           break;
		case EObjType::HUNTER:          GameObj = new CHunter;           break;
		case EObjType::MUNCHIE:         GameObj = new CMunchie;          break;
		case EObjType::PHRED:           GameObj = new CPhred;            break;
		case EObjType::POD:             GameObj = new CPod;              break;
		case EObjType::REFORMER:        GameObj = new CReformer;         break;
		case EObjType::REFORMERPART:    GameObj = new CReformerPart;     break;
		case EObjType::SPACEHUM:        GameObj = new CSpacehum;         break;
		case EObjType::SWARMER:         GameObj = new CSwarmer;          break;
		case EObjType::TURRET:          GameObj = new CTurret;           break;

		default: 
			check(false); 
			break;
	}

	return GameObj;
}



#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
