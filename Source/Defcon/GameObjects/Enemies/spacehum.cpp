// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	spacehum.cpp
	Space Hum enemy type for Defcon game.
*/


#include "spacehum.h"

#include "DefconUtils.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CSpacehum::CSpacehum()
{
	ParentType = Type;
	Type = EObjType::SPACEHUM;
	PointValue = SPACEHUM_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_LIGHT;

	AnimSpeed = FRAND * 0.05f + 0.15f;
	bCanBeInjured = true;
	bIsCollisionInjurious = true;
	Brightness = FRANDRANGE(0.9f, 1.0f);

	Speed = (float)GDefconGameInstance->GetScore() / 250;
	Speed *= FRANDRANGE(0.9f, 1.33f);

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


void Defcon::CSpacehum::Tick(float DeltaTime)
{
	// Just chase the player.

	CEnemy::Tick(DeltaTime);

	Inertia = Position;

	if(TargetPtr != nullptr)
	{
		GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);
		
		CFPoint Budge(sinf(FRAND * PI * 2), cosf(FRAND * PI * 2));

		// todo: initially fly away quickly from the parent dynamo toward player
		if(Age < 2.0f)
		{
			Budge.x *= 0.1f;
			Orientation.Fwd.x *= 0.1f;
		}

		Budge *= SFRAND * 200.0f * DeltaTime;
		Position.MulAdd(Orientation.Fwd, DeltaTime * Speed);
		Position += Budge;
	}

	Inertia = Position - Inertia;
}


void Defcon::CSpacehum::Explode(CGameObjectCollection& Debris)
{
	FExplosionParams Params;

	Params.NumParticles    = 10;
	Params.MaxParticleSize = 4;
	Params.YoungColor[0]   = 
	Params.YoungColor[1]   = EColor::Gray;

	AddExplosionDebris(Params, Debris);
}
