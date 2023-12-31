// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission.cpp
	IMission base class
	Defines commonalities of missions in Defence Condition.
*/


#include "mission.h"

#include "DefconUtils.h"
#include "DefconLogging.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/playership.h"
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



Defcon::IMission::IMission()
{
	UE_LOG(LogGame, Log, TEXT("Creating mission"));
}


Defcon::IMission::~IMission()
{
	UE_LOG(LogGame, Log, TEXT("Destroying mission"));
}


void Defcon::IMission::Init()
{
	GArena->OnMissionStarting();
}


bool Defcon::IMission::Tick(float DeltaTime) 
{
	Age += DeltaTime; 

	ScheduledTasks     .Process(DeltaTime);
	EnemyCreationTasks .Process(DeltaTime);

	DoIntroText(DeltaTime);

	return true; 
}


void Defcon::IMission::DoIntroText(float DeltaTime)
{
	if(IntroIsDone)
	{
		return;
	}
	
	IntroIsDone = true;

	TArray<FString> IntroTextLines;
	IntroText.ParseIntoArray(IntroTextLines, TEXT("\n"), false);

	for(const auto& IntroTextLine : IntroTextLines)
	{
		Defcon::GMessageMediator.TellUser(IntroTextLine);
	}
}


void Defcon::IMission::AddGameObject
(
	Defcon::EObjType             ObjType, 
	Defcon::EObjType             CreatorType, 
	const CFPoint&               Where, 
	float                        Countdown, 
	Defcon::EObjectCreationFlags Flags, 
	const FMetadata*             Options
)
{
	check(Where.IsValid());

	auto Task = new CCreateGameObjectTask;

	Task->ObjType			= ObjType;
	Task->CreatorType       = CreatorType;
	Task->Where				= Where;
	Task->bMissionTarget	= Daylon::HasFlag(Flags, EObjectCreationFlags::IsMissionTarget);
	Task->Countdown         = Countdown; 
	
	if(Options != nullptr)
	{
		Task->Options = *Options;
	}

	EnemyCreationTasks.Add(Task);
}




#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
