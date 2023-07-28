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
	while(!m_events.IsEmpty())
	{
		check(m_events.Last(0) != nullptr);
		
		delete m_events.Last(0);
		
		m_events.Pop();
	}
}


void Defcon::CEventQueue::Add(CEvent* pE)
{
	check(pE != nullptr);

	m_events.Add(pE);
}


void Defcon::CEventQueue::Process()
{
	const float now = GameTime();

	for(int32 Index = m_events.Num() - 1; Index >= 0; Index--)
	{
		auto EvtPtr = m_events[Index];

		if(EvtPtr->m_when <= now)
		{
			EvtPtr->Do();

			// If the event caused an arena transition, the event queue may have emptied, so check for that.
			if(m_events.IsEmpty())
			{
				return;
			}

			delete EvtPtr;

			m_events.RemoveAtSwap(Index);
		}
	}
}


void Defcon::CEventQueue::DeleteAll()
{
	for(auto EvtPtr : m_events)
	{
		check(EvtPtr != nullptr);
		
		delete EvtPtr;
	}
	m_events.Empty();
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
	SAFE_DELETE(m_pArena->Terrain);

	m_pArena->PlayAreaMain->TerrainPtr = nullptr;
	m_pArena->PlayAreaRadar->SetTerrain(nullptr);

	gDefconGameInstance->MissionEnded();
}

// --------------------------------------------------------


void Defcon::CCreateEnemyEvent::Do()
{
	// Create generic enemy.
	CEnemy* pE = this->CreateEnemy(m_objtype, m_where);

	check(pE != nullptr);


	// Specialize it.
	switch(m_objtype)
	{
		case ObjType::GHOST:			this->SpecializeForGhost             (pE, m_where); break;
		case ObjType::GHOSTPART:		this->SpecializeForGhostPart         (pE, m_where); break;
		case ObjType::GUPPY:			this->SpecializeForGuppy             (pE, m_where); break;
		case ObjType::HUNTER:			this->SpecializeForHunter            (pE, m_where); break;
		case ObjType::LANDER:			this->SpecializeForLander            (pE, m_where); break;
		case ObjType::BOMBER:			this->SpecializeForBomber            (pE, m_where); break;
		case ObjType::FIREBOMBER_TRUE:  this->SpecializeForFirebomber        (pE, m_where); break;
		case ObjType::FIREBOMBER_WEAK:  this->SpecializeForWeakFirebomber    (pE, m_where); break;
		case ObjType::FIREBALL:			this->SpecializeForFireball          (pE, m_where); break;
		case ObjType::DYNAMO:			this->SpecializeForDynamo            (pE, m_where); break;
		case ObjType::SPACEHUM:			this->SpecializeForSpacehum          (pE, m_where); break;
		case ObjType::BAITER:			this->SpecializeForBaiter            (pE, m_where); break;
		case ObjType::POD:				this->SpecializeForPod               (pE, m_where); break;
		case ObjType::SWARMER:			this->SpecializeForSwarmer           (pE, m_where); break;
		case ObjType::REFORMER:			this->SpecializeForReformer          (pE, m_where); break;
		case ObjType::REFORMERPART:		this->SpecializeForReformerPart      (pE, m_where); break;
		case ObjType::BOUNCER_TRUE:		this->SpecializeForBouncer           (pE, m_where); break;
		case ObjType::BOUNCER_WEAK:		this->SpecializeForWeakBouncer       (pE, m_where); break;
		case ObjType::PHRED:			this->SpecializeForPhred             (pE, m_where); break;
		case ObjType::BIGRED:			this->SpecializeForBigRed            (pE, m_where); break;
		case ObjType::MUNCHIE:			this->SpecializeForMunchie           (pE, m_where); break;
		case ObjType::TURRET:			this->SpecializeForTurret            (pE, m_where); break;

		default:
			check(false);
			break;
	}
	
	pE->WorldContextObject = gpArena;
	pE->InstallSprite();
	pE->SetAsMissionTarget(m_bMissionTarget);

	/*if(pE->IsMaterializing() && pE->Sprite)
	{
		pE->Sprite->Hide(); // Will be shown after materialization completes.
	}*/

	//if(!pE->IsMaterializing())
	//{
		pE->MakeHurtable();
		pE->SetCollisionInjurious();
	//}

	m_pArena->GetEnemies().Add(pE);

	pE->OnFinishedCreating();
}


Defcon::CEnemy* Defcon::CCreateEnemyEvent::CreateEnemy(ObjType kind, const CFPoint& where)
{
	// Generic enemy game object creation routine.

	CEnemy* pE = nullptr;

	switch(kind)
	{
		case ObjType::LANDER:			pE = new CLander;			break;
		case ObjType::GUPPY:			pE = new CGuppy;			break;
		case ObjType::HUNTER:			pE = new CHunter;			break;
		case ObjType::BOMBER:			pE = new CBomber;			break;
		case ObjType::BAITER:			pE = new CBaiter;			break;
		case ObjType::POD:				pE = new CPod;				break;
		case ObjType::SWARMER:			pE = new CSwarmer;			break;
		case ObjType::FIREBOMBER_TRUE:  pE = new CFirebomber;		break;
		case ObjType::FIREBOMBER_WEAK:  pE = new CWeakFirebomber;	break;
		case ObjType::FIREBALL:			pE = new CFireball;			break;
		case ObjType::DYNAMO:			pE = new CDynamo;			break;
		case ObjType::SPACEHUM:			pE = new CSpacehum;			break;
		case ObjType::REFORMER:			pE = new CReformer;			break;
		case ObjType::REFORMERPART:		pE = new CReformerPart;		break;
		case ObjType::BOUNCER_TRUE:  	pE = new CBouncer;			break;
		case ObjType::BOUNCER_WEAK:  	pE = new CWeakBouncer;		break;
		case ObjType::PHRED:			pE = new CPhred;			break;
		case ObjType::BIGRED:			pE = new CBigRed;			break;
		case ObjType::MUNCHIE:			pE = new CMunchie;			break;
		case ObjType::GHOST:			pE = new CGhost;			break;
		case ObjType::GHOSTPART:		pE = new CGhostPart;		break;
		//case ObjType::GHOSTPARTPRIMARY: pE = new CGhostPrimaryPart;	break;
		case ObjType::TURRET:			pE = new CTurret;			break;

		default: throw 0; break;
	}

	pE->m_pMapper = &m_pArena->GetMainAreaMapper();
	pE->m_pos = where;
	float w = m_pArena->GetDisplayWidth();

	pE->Init(CFPoint((float)m_pArena->GetWidth(), (float)m_pArena->GetHeight()), CFPoint(w, (float)m_pArena->GetHeight()));

	return pE;
}



#define SET_RANDOM_FWD_ORIENT   pE->m_orient.fwd.x = SBRAND;
#define SET_PLAYER_AS_TARGET    pE->SetTarget(&m_pArena->GetPlayerShip());


void Defcon::CCreateEnemyEvent::SpecializeForBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	pE->m_orient.fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForWeakBouncer(Defcon::CEnemy* pE, const CFPoint& where)
{
	pE->m_orient.fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateEnemyEvent::SpecializeForPhred(Defcon::CEnemy* pE, const CFPoint& where)
{
	//check(m_pArena == gpArena);
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForBigRed(Defcon::CEnemy* pE, const CFPoint& where)
{
	//check(m_pArena == gpArena);
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateEnemyEvent::SpecializeForMunchie(Defcon::CEnemy* pE, const CFPoint& where)
{
	//check(m_pArena == gpArena);
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}



void Defcon::CCreateEnemyEvent::SpecializeForGhost(Defcon::CEnemy* pE, const CFPoint& where)
{
	check(m_pArena == gpArena);
	CGhost* p = static_cast<CGhost*>(pE);
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateEnemyEvent::SpecializeForGhostPart(Defcon::CEnemy* pE, const CFPoint& where)
{
	check(m_pArena == gpArena);
	CGhostPart* p = static_cast<CGhostPart*>(pE);
	SET_PLAYER_AS_TARGET
	p->SetCollisionInjurious(false);
}

#if 0
void Defcon::CCreateEnemyEvent::SpecializeForGhostPartPrimary(Defcon::CEnemy* pE, const CFPoint& where)
{
	check(m_pArena == gpArena);
	CGhostPrimaryPart* p = static_cast<CGhostPrimaryPart*>(pE);
	p->SetTarget((IGameObject*)&m_pArena->GetPlayerShip());
	p->SetCollisionInjurious(false);
}
#endif

void Defcon::CCreateEnemyEvent::SpecializeForLander(Defcon::CEnemy* pE, const CFPoint& where)
{
	CLander* p = static_cast<CLander*>(pE);
	//p->m_fnTerrainEval = GetTerrainElevFunc;
	//p->m_fnHumanFinder = Defcon::FindHumanFunc;
	p->m_pvUserTerrainEval = m_pArena;
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &m_pArena->GetObjects();
	p->SetDoChaseHumans(gDefconGameInstance->GetMission()->HumansInvolved());
}


void Defcon::CCreateEnemyEvent::SpecializeForGuppy(Defcon::CEnemy* pE, const CFPoint& where)
{
	CGuppy* p = static_cast<CGuppy*>(pE);
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &m_pArena->GetEnemies();
}


void Defcon::CCreateEnemyEvent::SpecializeForHunter(Defcon::CEnemy* pE, const CFPoint& where)
{
	CHunter* p = static_cast<CHunter*>(pE);
	SET_PLAYER_AS_TARGET
	p->m_pObjects = &m_pArena->GetEnemies();
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
	//pE->m_pos.y = FRANDRANGE(90, m_pArena->GetHeight() - 90);
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
	pE->m_orient.fwd.x = SBRAND;
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
