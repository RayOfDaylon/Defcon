// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	bomber.cpp
	Bomber type for Defcon game.
*/


#include "bomber.h"

#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"

#include "GameObjects/obj_types.h"
#include "GameObjects/playership.h"

#include "Arenas/DefconPlayViewBase.h"

#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


#define SPLINE_WIDTH_MIN    1000.0f
#define SPLINE_WIDTH_MAX    1500.0f
#define SECONDS_PER_PATH_MIN   1.0f
#define SECONDS_PER_PATH_MAX   1.5f



Defcon::CBomber::CBomber()
	:
	StopLayingMinesCountdown(0.0f)
{
	ParentType = Type;
	Type       = EObjType::BOMBER;

	PointValue = BOMBER_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = MakeColorFromComponents(64, 0, 255);
	
	AnimSpeed = FRAND * 0.35f + 0.15f;

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);

	SecondsPerPath = Daylon::FRandRange(SECONDS_PER_PATH_MIN, SECONDS_PER_PATH_MAX);
}


void Defcon::CBomber::OnFinishedCreating(const Daylon::FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	if(Orientation.Fwd.x < 0)
	{
		// Use left-facing texture atlas.
		const auto& SpriteInfo = GGameObjectResources.Get(EObjType::BOMBER_LEFT);
		Sprite->SetAtlas(SpriteInfo.Atlas->Atlas);
	}


	PreviousPosition = Position;

	float VDir = SBRAND;

	// Make a spline we will march along

	// todo: s/b able to consolidate some code with Move()

	CurrentPath.Pts[0] = Position;

	CurrentPath.Pts[3] = Position + CFPoint(Orientation.Fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(ArenaSize.y / 5, ArenaSize.y / 2));
	CurrentPath.Pts[3].y = CLAMP(CurrentPath.Pts[3].y, 0.0f, ArenaSize.y);

	// If we make the control points meet up x-wise, the curve will mimic a sine wave.
	CurrentPath.Pts[1] = CFPoint((Position.x + CurrentPath.Pts[3].x) / 2, Position.y); 
	CurrentPath.Pts[2] = CFPoint(CurrentPath.Pts[1].x, CurrentPath.Pts[3].y); 
}


void Defcon::CBomber::Tick(float DeltaTime)
{
	// Move in slightly perturbed sine wave pattern.

	// Our explosions looked bad near the world origin because
	// although we don't wrap Position.x here, it does get wrapped 
	// during object processing right after Move(), and then 
	// our default Inertia = Position, Position = new pos,  Inertia = Position - Inertia fails because 
	// we do not increment Position, we compute it fresh from our current path which can be an unwrapped version.
	// This was fixed by tracking Position into PreviousPosition and basing inertia on their difference,
	// thus keeping the computation always in unwrapped space.

	CEnemy::Tick(DeltaTime);

	if(GArena->GetPlayerShip().IsAlive())
	{
		if(StopLayingMinesCountdown > 0.0f)
		{
			StopLayingMinesCountdown -= DeltaTime;

			if(FRAND < 0.2f)
			{
				GArena->LayMine(*this, Position, 1, 1);
			}
		}
		else if(
			FRAND <= 0.01f 
			&& CanBeInjured()
			&& IsOurPositionVisible()
			&& SGN(Orientation.Fwd.x) == SGN(GArena->GetPlayerShip().Orientation.Fwd.x))
		{
			StopLayingMinesCountdown = FRANDRANGE(0.33f, 0.5f);
		}
	}



	float T = PathTravelTime / SecondsPerPath;

	T = CLAMP(T, 0.0f, 1.0f);

	CurrentPath.CalcPt(T, Position);

	PathTravelTime += DeltaTime;

	if(PathTravelTime > SecondsPerPath)
	{
		do { PathTravelTime -= SecondsPerPath; } while(PathTravelTime > SecondsPerPath);

		// Set up our next flight path.
		// We always toggle vertical direction.
		float VDir = -(SGN(CurrentPath.Pts[3].y - CurrentPath.Pts[0].y));

		// Wrap path endpoint so we don't eventually get way off arena.
		CurrentPath.Pts[3].x = GArena->WrapX(CurrentPath.Pts[3].x);

		CurrentPath.Pts[0] = CurrentPath.Pts[3];

		CurrentPath.Pts[3] = CurrentPath.Pts[0] + CFPoint(Orientation.Fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(ArenaSize.y / 5, ArenaSize.y / 2));
		CurrentPath.Pts[3].y = CLAMP(CurrentPath.Pts[3].y, 0.0f, ArenaSize.y);

		// If we make the control points meet up x-wise, the curve will mimic a sine wave.
		CurrentPath.Pts[1] = CFPoint((CurrentPath.Pts[0].x + CurrentPath.Pts[3].x) / 2, Position.y); 
		CurrentPath.Pts[2] = CFPoint(CurrentPath.Pts[1].x, CurrentPath.Pts[3].y); 
	}

	Inertia = Position - PreviousPosition;
	PreviousPosition = Position;
}


Defcon::EColor Defcon::CBomber::GetExplosionColorBase() const
{
	return EColor::LightBlue;
}


float Defcon::CBomber::GetExplosionMass() const
{
	return 0.75f;
}

#undef SPLINE_WIDTH_MIN
#undef SPLINE_WIDTH_MAX
#undef SECONDS_PER_PATH
#undef SECONDS_PER_PATH_MIN
#undef SECONDS_PER_PATH_MAX

#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
