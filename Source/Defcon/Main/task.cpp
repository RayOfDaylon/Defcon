// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	task.cpp
	Defcon scheduled task system.
	Scheduled tasks are used to easily run code that needs to occur later, 
	e.g. enemy spawns, arena closures, etc.
*/

#include "task.h"

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



Defcon::CScheduledTaskList::~CScheduledTaskList()
{
	while(!Tasks.IsEmpty())
	{
		check(Tasks.Last(0) != nullptr);
		
		delete Tasks.Last(0);
		
		Tasks.Pop();
	}
}


void Defcon::CScheduledTaskList::Add(CScheduledTask* Task)
{
	check(Task != nullptr);

	Tasks.Add(Task);
}


void Defcon::CScheduledTaskList::Process(float DeltaTime)
{
	for(int32 Index = Tasks.Num() - 1; Index >= 0; Index--)
	{
		auto Task = Tasks[Index];

		Task->Countdown -= DeltaTime;

		if(Task->Countdown <= 0.0f)
		{
			Task->Do();

			// If the task was major then the task list may have emptied, so check for that.
			if(Tasks.IsEmpty())
			{
				return;
			}

			delete Task;

			Tasks.RemoveAtSwap(Index);
		}
	}
}


void Defcon::CScheduledTaskList::DeleteAll()
{
	for(auto Task : Tasks)
	{
		check(Task != nullptr);
		
		delete Task;
	}
	Tasks.Empty();
}


void Defcon::CScheduledTaskList::ForEach(TFunction<void(CScheduledTask*)> Function) const
{
	check(Function);

	for(auto Task : Tasks)
	{
		Function(Task);
	}
}


void Defcon::CScheduledTaskList::ForEachUntil(TFunction<bool(CScheduledTask*)> Function) const
{
	check(Function);

	for(auto Task : Tasks)
	{
		if(!Function(Task))
		{
			break;
		}
	}
}

// --------------------------------------------------------

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
	SAFE_DELETE(GArena->Terrain);

	GArena->PlayAreaMain ->SetTerrain(nullptr);
	GArena->PlayAreaRadar->SetTerrain(nullptr);

	Defcon::GGameMatch->MissionEnded();
}

// --------------------------------------------------------


void Defcon::CCreateGameObjectTask::Do()
{
	// Create generic enemy.

	CEnemy* Enemy = CreateEnemy();

	check(Enemy != nullptr);


	// Specialize it.
	switch(ObjType)
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


Defcon::CEnemy* Defcon::CCreateGameObjectTask::CreateEnemy()
{
	// Generic enemy game object creation routine.

	CEnemy* Enemy = nullptr;

	switch(ObjType)
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

	Enemy->Init(CFPoint(GArena->GetWidth(),        GArena->GetHeight()), 
	            CFPoint(GArena->GetDisplayWidth(), GArena->GetHeight()));

	return Enemy;
}



#define SET_RANDOM_FWD_ORIENT   Enemy->Orientation.Fwd.x = SBRAND;
#define SET_PLAYER_AS_TARGET    Enemy->SetTarget(&GArena->GetPlayerShip());


void Defcon::CCreateGameObjectTask::SpecializeForBouncer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateGameObjectTask::SpecializeForWeakBouncer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = (FRAND * 15 + 5) * (BRAND ? -1 : 1);
}


void Defcon::CCreateGameObjectTask::SpecializeForPhred(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForBigRed(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForMunchie(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForGhost(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateGameObjectTask::SpecializeForGhostPart(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->SetCollisionInjurious(false);
}


void Defcon::CCreateGameObjectTask::SpecializeForLander(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET

	CLander* p = static_cast<CLander*>(Enemy);
	
	p->Objects = &GArena->GetObjects();
	p->SetDoTryToAbduct(Defcon::GGameMatch->GetMission()->HumansInvolved());
}


void Defcon::CCreateGameObjectTask::SpecializeForGuppy(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateGameObjectTask::SpecializeForHunter(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateGameObjectTask::SpecializeForBomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForFirebomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForWeakFirebomber(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForFireball(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateGameObjectTask::SpecializeForDynamo(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForSpacehum(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
}


void Defcon::CCreateGameObjectTask::SpecializeForPod(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForSwarmer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	Enemy->Orientation.Fwd.x = SBRAND;

	CSwarmer* p = static_cast<CSwarmer*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateGameObjectTask::SpecializeForTurret(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT

	auto p = static_cast<CTurret*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateGameObjectTask::SpecializeForReformerPart(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
	CReformerPart* p = static_cast<CReformerPart*>(Enemy);
	p->SetOriginalPosition(where);
}


void Defcon::CCreateGameObjectTask::SpecializeForBaiter(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


void Defcon::CCreateGameObjectTask::SpecializeForReformer(Defcon::CEnemy* Enemy, const CFPoint& where)
{
	SET_PLAYER_AS_TARGET
	SET_RANDOM_FWD_ORIENT
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
