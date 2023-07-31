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
	auto GI = gDefconGameInstance;

	GI->SetCurrentMission(GI->GetMission()->GetID());
	GI->TransitionToArena(EDefconArena::Prewave);
}

// -----------------------------------------------------

void Defcon::CEndMissionEvent::Do()
{
	SAFE_DELETE(gpArena->Terrain);

	gpArena->PlayAreaMain->TerrainPtr = nullptr;
	gpArena->PlayAreaRadar->SetTerrain(nullptr);

	gDefconGameInstance->MissionEnded();
}

// --------------------------------------------------------


void Defcon::CCreateEnemyEvent::Do()
{
	// Create generic enemy.
	CEnemy* pE = this->CreateEnemy(EnemyType, Where);

	check(pE != nullptr);


	// Specialize it.
	switch(EnemyType)
	{
		case EObjType::GHOST:           this->SpecializeForGhost             (pE, Where); break;
		case EObjType::GHOSTPART:       this->SpecializeForGhostPart         (pE, Where); break;
		case EObjType::GUPPY:           this->SpecializeForGuppy             (pE, Where); break;
		case EObjType::HUNTER:          this->SpecializeForHunter            (pE, Where); break;
		case EObjType::LANDER:          this->SpecializeForLander            (pE, Where); break;
		case EObjType::BOMBER:          this->SpecializeForBomber            (pE, Where); break;
		case EObjType::FIREBOMBER_TRUE: this->SpecializeForFirebomber        (pE, Where); break;
		case EObjType::FIREBOMBER_WEAK: this->SpecializeForWeakFirebomber    (pE, Where); break;
		case EObjType::FIREBALL:        this->SpecializeForFireball          (pE, Where); break;
		case EObjType::DYNAMO:          this->SpecializeForDynamo            (pE, Where); break;
		case EObjType::SPACEHUM:        this->SpecializeForSpacehum          (pE, Where); break;
		case EObjType::BAITER:          this->SpecializeForBaiter            (pE, Where); break;
		case EObjType::POD:             this->SpecializeForPod               (pE, Where); break;
		case EObjType::SWARMER:         this->SpecializeForSwarmer           (pE, Where); break;
		case EObjType::REFORMER:        this->SpecializeForReformer          (pE, Where); break;
		case EObjType::REFORMERPART:    this->SpecializeForReformerPart      (pE, Where); break;
		case EObjType::BOUNCER_TRUE:    this->SpecializeForBouncer           (pE, Where); break;
		case EObjType::BOUNCER_WEAK:    this->SpecializeForWeakBouncer       (pE, Where); break;
		case EObjType::PHRED:           this->SpecializeForPhred             (pE, Where); break;
		case EObjType::BIGRED:          this->SpecializeForBigRed            (pE, Where); break;
		case EObjType::MUNCHIE:         this->SpecializeForMunchie           (pE, Where); break;
		case EObjType::TURRET:          this->SpecializeForTurret            (pE, Where); break;

		default:
			check(false);
			break;
	}
	
	pE->InstallSprite();
	pE->SetAsMissionTarget(bMissionTarget);
	pE->MakeHurtable();
	pE->SetCollisionInjurious();

	gpArena->GetEnemies().Add(pE);

	pE->OnFinishedCreating();
}


Defcon::CEnemy* Defcon::CCreateEnemyEvent::CreateEnemy(EObjType kind, const CFPoint& where)
{
	// Generic enemy game object creation routine.

	CEnemy* pE = nullptr;

	switch(kind)
	{
		case EObjType::LANDER:			pE = new CLander;			break;
		case EObjType::GUPPY:			pE = new CGuppy;			break;
		case EObjType::HUNTER:			pE = new CHunter;			break;
		case EObjType::BOMBER:			pE = new CBomber;			break;
		case EObjType::BAITER:			pE = new CBaiter;			break;
		case EObjType::POD:				pE = new CPod;				break;
		case EObjType::SWARMER:			pE = new CSwarmer;			break;
		case EObjType::FIREBOMBER_TRUE:  pE = new CFirebomber;		break;
		case EObjType::FIREBOMBER_WEAK:  pE = new CWeakFirebomber;	break;
		case EObjType::FIREBALL:			pE = new CFireball;			break;
		case EObjType::DYNAMO:			pE = new CDynamo;			break;
		case EObjType::SPACEHUM:			pE = new CSpacehum;			break;
		case EObjType::REFORMER:			pE = new CReformer;			break;
		case EObjType::REFORMERPART:		pE = new CReformerPart;		break;
		case EObjType::BOUNCER_TRUE:  	pE = new CBouncer;			break;
		case EObjType::BOUNCER_WEAK:  	pE = new CWeakBouncer;		break;
		case EObjType::PHRED:			pE = new CPhred;			break;
		case EObjType::BIGRED:			pE = new CBigRed;			break;
		case EObjType::MUNCHIE:			pE = new CMunchie;			break;
		case EObjType::GHOST:			pE = new CGhost;			break;
		case EObjType::GHOSTPART:		pE = new CGhostPart;		break;
		//case EObjType::GHOSTPARTPRIMARY: pE = new CGhostPrimaryPart;	break;
		case EObjType::TURRET:			pE = new CTurret;			break;

		default: throw 0; break;
	}

	pE->MapperPtr = &gpArena->GetMainAreaMapper();
	pE->Position = where;
	float w = gpArena->GetDisplayWidth();

	pE->Init(CFPoint((float)gpArena->GetWidth(), (float)gpArena->GetHeight()), CFPoint(w, (float)gpArena->GetHeight()));

	return pE;
}



#define SET_RANDOM_FWD_ORIENT   pE->Orientation.fwd.x = SBRAND;
#define SET_PLAYER_AS_TARGET    pE->SetTarget(&gpArena->GetPlayerShip());


void Defcon::CCreateEnemyEvent::SpecializeForBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	pE->Orientation.fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	pE->Orientation.fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
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
	CGhost* p = static_cast<CGhost*>(pE);
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForGhostPart(Defcon::CEnemy* pE, const CFPoint& where)
{
	CGhostPart* p = static_cast<CGhostPart*>(pE);
	SET_PLAYER_AS_TARGET
	p->SetCollisionInjurious(false);
}

#if 0
void Defcon::CCreateEnemyEvent::SpecializeForGhostPartPrimary(Defcon::CEnemy* pE, const CFPoint& where)
{
	CGhostPrimaryPart* p = static_cast<CGhostPrimaryPart*>(pE);
	p->SetTarget((IGameObject*)&gpArena->GetPlayerShip());
	p->SetCollisionInjurious(false);
}
#endif

void Defcon::CCreateEnemyEvent::SpecializeForLander(Defcon::CEnemy* pE, const CFPoint& where)
{
	CLander* p = static_cast<CLander*>(pE);
	//p->m_fnTerrainEval = GetTerrainElevFunc;
	//p->m_fnHumanFinder = Defcon::FindHumanFunc;
	p->m_pvUserTerrainEval = gpArena;
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &gpArena->GetObjects();
	p->SetDoChaseHumans(gDefconGameInstance->GetMission()->HumansInvolved());
}


void Defcon::CCreateEnemyEvent::SpecializeForGuppy(Defcon::CEnemy* pE, const CFPoint& where)
{
	CGuppy* p = static_cast<CGuppy*>(pE);
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &gpArena->GetEnemies();
}


void Defcon::CCreateEnemyEvent::SpecializeForHunter(Defcon::CEnemy* pE, const CFPoint& where)
{
	CHunter* p = static_cast<CHunter*>(pE);
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &gpArena->GetEnemies();
}


void Defcon::CCreateEnemyEvent::SpecializeForBomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFirebomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakFirebomber(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForFireball(Defcon::CEnemy* pE, const CFPoint& where)
{
}


void Defcon::CCreateEnemyEvent::SpecializeForDynamo(Defcon::CEnemy* pE, const CFPoint& where)
{
	//pE->Position.y = FRANDRANGE(90, gpArena->GetHeight() - 90);
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSpacehum(Defcon::CEnemy* pE, const CFPoint& where)
{
}


void Defcon::CCreateEnemyEvent::SpecializeForPod(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForSwarmer(Defcon::CEnemy* pE, const CFPoint& where)
{
	CSwarmer* p = static_cast<CSwarmer*>(pE);
	SET_PLAYER_AS_TARGET
	pE->Orientation.fwd.x = SBRAND;
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForTurret(Defcon::CEnemy* pE, const CFPoint& where)
{
	auto p = static_cast<CTurret*>(pE);
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForReformerPart(Defcon::CEnemy* pE, const CFPoint& where)
{
	CReformerPart* p = static_cast<CReformerPart*>(pE);
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
	p->SetOriginalPosition(where);
}


void Defcon::CCreateEnemyEvent::SpecializeForBaiter(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForReformer(Defcon::CEnemy* pE, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
