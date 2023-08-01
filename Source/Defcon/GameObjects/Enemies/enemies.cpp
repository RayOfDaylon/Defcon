// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	enemies.cpp
	Enemy base types for Defcon game.
*/


#include "enemies.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"

#include "Arenas/DefconPlayViewBase.h"

#include "DefconLogging.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Defcon::CEnemy::CEnemy()
{
	ParentType = Type;
	Type       = EObjType::ENEMY;

	bCanBeInjured         = false;
	bIsCollisionInjurious = false;
	Brightness            = FRAND * 0.33f + 0.66f;
}


Defcon::CEnemy::~CEnemy()
{
}


void Defcon::CEnemy::Move(float f)
{
	CFPoint temp = Inertia;
	Super::Move(f);
	Inertia = temp;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
