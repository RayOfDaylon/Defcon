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


void Defcon::CEventQueue::Add(CEvent* pE)
{
	check(pE != nullptr);

	Events.Add(pE);
}


void Defcon::CEventQueue::Process(float DeltaTime)
{
	const float now = GameTime();

	for(int32 Index = Events.Num() - 1; Index >= 0; Index--)
	{
		auto EvtPtr = Events[Index];

		if(EvtPtr->When <= now)
		{
			EvtPtr->Do();

			// If the event caused an arena transition, the event queue may have emptied, so check for that.
			if(Events.IsEmpty())
			{
				return;
			}

			delete EvtPtr;

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
	CEnemy* EnemyPtr = CreateEnemy(EnemyType, Where);

	check(EnemyPtr != nullptr);


	// Specialize it.
	switch(EnemyType)
	{
		case EObjType::GHOST:           SpecializeForGhost          (EnemyPtr, Where); break;
		case EObjType::GHOSTPART:       SpecializeForGhostPart      (EnemyPtr, Where); break;
		case EObjType::GUPPY:           SpecializeForGuppy          (EnemyPtr, Where); break;
		case EObjType::HUNTER:          SpecializeForHunter         (EnemyPtr, Where); break;
		case EObjType::LANDER:          SpecializeForLander         (EnemyPtr, Where); break;
		case EObjType::BOMBER:          SpecializeForBomber         (EnemyPtr, Where); break;
		case EObjType::FIREBOMBER_TRUE: SpecializeForFirebomber     (EnemyPtr, Where); break;
		case EObjType::FIREBOMBER_WEAK: SpecializeForWeakFirebomber (EnemyPtr, Where); break;
		case EObjType::FIREBALL:        SpecializeForFireball       (EnemyPtr, Where); break;
		case EObjType::DYNAMO:          SpecializeForDynamo         (EnemyPtr, Where); break;
		case EObjType::SPACEHUM:        SpecializeForSpacehum       (EnemyPtr, Where); break;
		case EObjType::BAITER:          SpecializeForBaiter         (EnemyPtr, Where); break;
		case EObjType::POD:             SpecializeForPod            (EnemyPtr, Where); break;
		case EObjType::SWARMER:         SpecializeForSwarmer        (EnemyPtr, Where); break;
		case EObjType::REFORMER:        SpecializeForReformer       (EnemyPtr, Where); break;
		case EObjType::REFORMERPART:    SpecializeForReformerPart   (EnemyPtr, Where); break;
		case EObjType::BOUNCER_TRUE:    SpecializeForBouncer        (EnemyPtr, Where); break;
		case EObjType::BOUNCER_WEAK:    SpecializeForWeakBouncer    (EnemyPtr, Where); break;
		case EObjType::PHRED:           SpecializeForPhred          (EnemyPtr, Where); break;
		case EObjType::BIGRED:          SpecializeForBigRed         (EnemyPtr, Where); break;
		case EObjType::MUNCHIE:         SpecializeForMunchie        (EnemyPtr, Where); break;
		case EObjType::TURRET:          SpecializeForTurret         (EnemyPtr, Where); break;

		default:
			check(false);
			break;
	}
	
	EnemyPtr->InstallSprite();
	EnemyPtr->SetAsMissionTarget(bMissionTarget);
	EnemyPtr->MakeHurtable();
	EnemyPtr->SetCollisionInjurious();

	GArena->GetEnemies().Add(EnemyPtr);

	EnemyPtr->OnFinishedCreating();
}


Defcon::CEnemy* Defcon::CCreateEnemyEvent::CreateEnemy(EObjType kind, const CFPoint& where)
{
	// Generic enemy game object creation routine.

	CEnemy* pE = nullptr;

	switch(kind)
	{
		case EObjType::LANDER:          pE = new CLander;           break;
		case EObjType::GUPPY:           pE = new CGuppy;            break;
		case EObjType::HUNTER:          pE = new CHunter;           break;
		case EObjType::BOMBER:          pE = new CBomber;           break;
		case EObjType::BAITER:          pE = new CBaiter;           break;
		case EObjType::POD:             pE = new CPod;              break;
		case EObjType::SWARMER:         pE = new CSwarmer;          break;
		case EObjType::FIREBOMBER_TRUE: pE = new CFirebomber;       break;
		case EObjType::FIREBOMBER_WEAK: pE = new CWeakFirebomber;   break;
		case EObjType::FIREBALL:        pE = new CFireball;         break;
		case EObjType::DYNAMO:          pE = new CDynamo;           break;
		case EObjType::SPACEHUM:        pE = new CSpacehum;         break;
		case EObjType::REFORMER:        pE = new CReformer;         break;
		case EObjType::REFORMERPART:    pE = new CReformerPart;     break;
		case EObjType::BOUNCER_TRUE:    pE = new CBouncer;          break;
		case EObjType::BOUNCER_WEAK:    pE = new CWeakBouncer;      break;
		case EObjType::PHRED:           pE = new CPhred;            break;
		case EObjType::BIGRED:          pE = new CBigRed;           break;
		case EObjType::MUNCHIE:         pE = new CMunchie;          break;
		case EObjType::GHOST:           pE = new CGhost;            break;
		case EObjType::GHOSTPART:       pE = new CGhostPart;        break;
		case EObjType::TURRET:          pE = new CTurret;           break;

		default: throw 0; break;
	}

	pE->MapperPtr = &GArena->GetMainAreaMapper();
	pE->Position = where;
	float w = GArena->GetDisplayWidth();

	pE->Init(CFPoint((float)GArena->GetWidth(), (float)GArena->GetHeight()), CFPoint(w, (float)GArena->GetHeight()));

	return pE;
}



#define SET_RANDOM_FWD_ORIENT   pE->Orientation.Fwd.x = SBRAND;
#define SET_PLAYER_AS_TARGET    pE->SetTarget(&GArena->GetPlayerShip());


void Defcon::CCreateEnemyEvent::SpecializeForBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	pE->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	pE->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForPhred(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForBigRed(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForMunchie(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForGhost(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForGhostPart(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	pE->SetCollisionInjurious(false);
}


void Defcon::CCreateEnemyEvent::SpecializeForLander(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET

	CLander* p = static_cast<CLander*>(pE);
	//p->m_pvUserTerrainEval = gpArena;
	p->Objects = &GArena->GetObjects();
	p->SetDoChaseHumans(GDefconGameInstance->GetMission()->HumansInvolved());
}


void Defcon::CCreateEnemyEvent::SpecializeForGuppy(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForHunter(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForBomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFirebomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakFirebomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFireball(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForDynamo(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSpacehum(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForPod(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSwarmer(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	pE->Orientation.Fwd.x = SBRAND;

	CSwarmer* p = static_cast<CSwarmer*>(pE);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForTurret(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT

	auto p = static_cast<CTurret*>(pE);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForReformerPart(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
	CReformerPart* p = static_cast<CReformerPart*>(pE);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForBaiter(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForReformer(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
