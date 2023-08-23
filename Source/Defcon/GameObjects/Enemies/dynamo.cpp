// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	dynamo.cpp
	Dynamo enemy type for Defcon game.
*/


#include "dynamo.h"

#include "DefconUtils.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CDynamo::CDynamo()
{
	ParentType = Type;
	Type       = EObjType::DYNAMO;

	PointValue = DYNAMO_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_LIGHT;
	AnimSpeed = FRAND * 0.05f + 0.15f;

	WiggleAnimSpeed.Set(FRANDRANGE(4.0f, 8.0f), FRANDRANGE(2.0f, 6.0f));

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius = SpriteInfo.Size / 2;
}


Defcon::CDynamo::~CDynamo()
{
}

#ifdef _DEBUG
const char* Defcon::CDynamo::GetClassname() const
{
	static char* psz = "Dynamo";
	return psz;
};
#endif


void Defcon::CDynamo::Tick(float DeltaTime)
{
	// Just float around drifting horizontally.

	if(Age == 0.0f)
	{
		OriginalY = Position.y;
	}

	CEnemy::Tick(DeltaTime);

	Inertia = Position;

	SpawnSpacehumCountdown -= DeltaTime;

	if(SpawnSpacehumCountdown <= 0.0f 
		&& CanBeInjured()
		&& GArena->GetPlayerShip().IsAlive()
		&& IsOurPositionVisible())
	{
		GArena->CreateEnemy(EObjType::SPACEHUM, GetType(), Position, 0.0f, EObjectCreationFlags::EnemyPart);

		const auto XP = GDefconGameInstance->GetScore();
		float T = NORM_(XP, 0.0f, 50000.0f);
		T = CLAMP(T, 0.0f, 1.0f);

		SpawnSpacehumCountdown = LERP(2.5f, 0.5f, T);
		SpawnSpacehumCountdown += Daylon::FRandRange(0.0f, 0.25f);

		// todo: maybe have dynamo take a break after spitting out lots of hums, then resume.
	}

	Position.x += sinf(WiggleAnimSpeed.x * Age) * 1.5f;
	
	const float Sn = sinf(WiggleAnimSpeed.y * Age) * 15.0f;
	Position.y = OriginalY + Sn;

	Position.y = CLAMP(Position.y, 0, GArena->GetHeight() - 1);

	Inertia = Position - Inertia;
}
