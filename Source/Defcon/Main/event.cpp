// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	event.cpp
	Defcon event system
*/

#include "event.h"

#include "DefconUtils.h"

#include "Arenas/DefconPlayViewBase.h"
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



Defcon::CEventQueue::~CEventQueue()
{
	while(!Events.IsEmpty())
	{
		check(Events.Last(0) != nullptr);
		
		delete Events.Last(0);
		
		Events.Pop();
	}
}


void Defcon::CEventQueue::Add(CEvent* Event)
{
	check(Event != nullptr);

	Events.Add(Event);
}


void Defcon::CEventQueue::Process(float DeltaTime)
{
	for(int32 Index = Events.Num() - 1; Index >= 0; Index--)
	{
		auto Event = Events[Index];

		Event->Countdown -= DeltaTime;

		if(Event->Countdown <= 0.0f)
		{
			Event->Do();

			// If the event caused an arena transition, the event queue may have emptied, so check for that.
			if(Events.IsEmpty())
			{
				return;
			}

			delete Event;

			Events.RemoveAtSwap(Index);
		}
	}
}


void Defcon::CEventQueue::DeleteAll()
{
	for(auto EvtPtr : Events)
	{
		check(EvtPtr != nullptr);
		
		delete EvtPtr;
	}
	Events.Empty();
}


// --------------------------------------------------------

void Defcon::CRestartMissionEvent::Do()
{
	auto GI = GDefconGameInstance;

	GI->SetCurrentMission(GI->GetMission()->GetID());
	GI->TransitionToArena(EDefconArena::Prewave);
}

// -----------------------------------------------------

void Defcon::CEndMissionEvent::Do()
{
	SAFE_DELETE(GArena->Terrain);

	GArena->PlayAreaMain->TerrainPtr = nullptr;
	GArena->PlayAreaRadar->SetTerrain(nullptr);

	GDefconGameInstance->MissionEnded();
}

// --------------------------------------------------------


void Defcon::CCreateEnemyEvent::Do()
{
	// Create generic enemy.

	CEnemy* Enemy = CreateEnemy();

	check(Enemy != nullptr);


	// Specialize it.
	switch(EnemyType)
	{
		case EObjType::GHOST:           SpecializeForGhost          (Enemy, Where); break;
		case EObjType::GHOSTPART:       SpecializeForGhostPart      (Enemy, Where); break;
		case EObjType::GUPPY:           SpecializeForGuppy          (Enemy, Where); break;
		case EObjType::HUNTER:          SpecializeForHunter         (Enemy, Where); break;
		case EObjType::LANDER:          SpecializeForLander         (Enemy, Where); break;
		case EObjType::BOMBER:          SpecializeForBomber         (Enemy, Where); break;
		case EObjType::FIREBOMBER_TRUE: SpecializeForFirebomber     (Enemy, Where); break;
		case EObjType::FIREBOMBER_WEAK: SpecializeForWeakFirebomber (Enemy, Where); break;
		case EObjType::FIREBALL:        SpecializeForFireball       (Enemy, Where); break;
		case EObjType::DYNAMO:          SpecializeForDynamo         (Enemy, Where); break;
		case EObjType::SPACEHUM:        SpecializeForSpacehum       (Enemy, Where); break;
		case EObjType::BAITER:          SpecializeForBaiter         (Enemy, Where); break;
		case EObjType::POD:             SpecializeForPod            (Enemy, Where); break;
		case EObjType::SWARMER:         SpecializeForSwarmer        (Enemy, Where); break;
		case EObjType::REFORMER:        SpecializeForReformer       (Enemy, Where); break;
		case EObjType::REFORMERPART:    SpecializeForReformerPart   (Enemy, Where); break;
		case EObjType::BOUNCER_TRUE:    SpecializeForBouncer        (Enemy, Where); break;
		case EObjType::BOUNCER_WEAK:    SpecializeForWeakBouncer    (Enemy, Where); break;
		case EObjType::PHRED:           SpecializeForPhred          (Enemy, Where); break;
		case EObjType::BIGRED:          SpecializeForBigRed         (Enemy, Where); break;
		case EObjType::MUNCHIE:         SpecializeForMunchie        (Enemy, Where); break;
		case EObjType::TURRET:          SpecializeForTurret         (Enemy, Where); break;

		default:
			check(false);
			break;
	}
	
	Enemy->InstallSprite();
	Enemy->SetAsMissionTarget(bMissionTarget);
	Enemy->MakeHurtable();
	Enemy->SetCollisionInjurious();

	GArena->GetEnemies().Add(Enemy);

	Enemy->OnFinishedCreating();
}


Defcon::CEnemy* Defcon::CCreateEnemyEvent::CreateEnemy()
{
	// Generic enemy game object creation routine.

	CEnemy* Enemy = nullptr;

	switch(EnemyType)
	{
		case EObjType::LANDER:          Enemy = new CLander;           break;
		case EObjType::GUPPY:           Enemy = new CGuppy;            break;
		case EObjType::HUNTER:          Enemy = new CHunter;           break;
		case EObjType::BOMBER:          Enemy = new CBomber;           break;
		case EObjType::BAITER:          Enemy = new CBaiter;           break;
		case EObjType::POD:             Enemy = new CPod;              break;
		case EObjType::SWARMER:         Enemy = new CSwarmer;          break;
		case EObjType::FIREBOMBER_TRUE: Enemy = new CFirebomber;       break;
		case EObjType::FIREBOMBER_WEAK: Enemy = new CWeakFirebomber;   break;
		case EObjType::FIREBALL:        Enemy = new CFireball;         break;
		case EObjType::DYNAMO:          Enemy = new CDynamo;           break;
		case EObjType::SPACEHUM:        Enemy = new CSpacehum;         break;
		case EObjType::REFORMER:        Enemy = new CReformer;         break;
		case EObjType::REFORMERPART:    Enemy = new CReformerPart;     break;
		case EObjType::BOUNCER_TRUE:    Enemy = new CBouncer;          break;
		case EObjType::BOUNCER_WEAK:    Enemy = new CWeakBouncer;      break;
		case EObjType::PHRED:           Enemy = new CPhred;            break;
		case EObjType::BIGRED:          Enemy = new CBigRed;           break;
		case EObjType::MUNCHIE:         Enemy = new CMunchie;          break;
		case EObjType::GHOST:           Enemy = new CGhost;            break;
		case EObjType::GHOSTPART:       Enemy = new CGhostPart;        break;
		case EObjType::TURRET:          Enemy = new CTurret;           break;

		default: 
			check(false); 
			break;
	}

	Enemy->SetCreatorType(CreatorType);
	Enemy->MapperPtr = &GArena->GetMainAreaMapper();
	Enemy->Position = Where;

	float w = GArena->GetDisplayWidth();

	Enemy->Init(CFPoint((float)GArena->GetWidth(), (float)GArena->GetHeight()), CFPoint(w, (float)GArena->GetHeight()));

	return Enemy;
}



#define SET_RANDOM_FWD_ORIENT   Enemy->Orientation.Fwd.x = SBRAND;
#define SET_PLAYER_AS_TARGET    Enemy->SetTarget(&GArena->GetPlayerShip());


void Defcon::CCreateEnemyEvent::SpecializeForBouncer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakBouncer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForPhred(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForBigRed(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForMunchie(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForGhost(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForGhostPart(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->SetCollisionInjurious(false);
}


void Defcon::CCreateEnemyEvent::SpecializeForLander(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET

	CLander* p = static_cast<CLander*>(Enemy);
	//p->m_pvUserTerrainEval = gpArena;
	p->Objects = &GArena->GetObjects();
	p->SetDoChaseHumans(GDefconGameInstance->GetMission()->HumansInvolved());
}


void Defcon::CCreateEnemyEvent::SpecializeForGuppy(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForHunter(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForBomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFirebomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakFirebomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFireball(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForDynamo(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSpacehum(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForPod(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSwarmer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = SBRAND;

	CSwarmer* p = static_cast<CSwarmer*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForTurret(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT

	auto p = static_cast<CTurret*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForReformerPart(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
	CReformerPart* p = static_cast<CReformerPart*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForBaiter(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForReformer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
