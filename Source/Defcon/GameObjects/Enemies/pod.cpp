// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	pod.cpp
	Pod enemy type for Defcon game.
*/

#include "pod.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"
#include "DefconLogging.h"



Defcon::CPod::CPod()
	:
	OffsetY(0.0f),
	Frequency(2.0f)
{
	ParentType = Type;
	Type       = EObjType::POD;

	PointValue = POD_VALUE;
	RadarColor = C_MAGENTA;

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


void Defcon::CPod::OnFinishedCreating(const Daylon::FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	check(Options.Map.Num() >= 2);

	Orientation.Fwd.x = Options.GetReal(TEXT("OrientationX"));
	SetSpeed(Options.GetReal(TEXT("SpeedX")));
}


Defcon::CPod::~CPod()
{
}


void Defcon::CPod::Tick(float DeltaTime)
{
	// Just float around drifting horizontally.

	CEnemy::Tick(DeltaTime);

	Inertia = Position;

	//UE_LOG(LogGame, Log, TEXT("%S: Pod is at %d, %d"), __FUNCTION__, (int32)Position.x, (int32)Position.y);

	// todo: pods actually move vertically in a linear fashion and wraparound vertically.
	Orientation.Fwd.y = 0.1f * sinf(Frequency * (OffsetY + Age)); 

	// Cause radar blip to blink
	const float T = PSIN(Age * PI * 2.0f);
	LerpColor(C_RED, C_BLUE, T, RadarColor);


	Position.MulAdd(Orientation.Fwd, Speed * DeltaTime);

	//UE_LOG(LogGame, Log, TEXT("%S: Pod now at %d, %d"), __FUNCTION__, (int32)Position.x, (int32)Position.y);

	Inertia = Position - Inertia;
}


void Defcon::CPod::OnAboutToDie()
{
	// Release swarmers.

	for(int32 I = 0; I < SWARMERS_PER_POD; I++)
	{
		GArena->SpawnGameObject(EObjType::SWARMER, GetType(), Position, 0.0f, EObjectCreationFlags::EnemyPart);
	}
}


void Defcon::CPod::Explode(CGameObjectCollection& Debris)
{
	// Pods explode normally (but deep purple or red)
	// but if collided with perfectly, even more so because 
	// all the swarmers are biting it.

	FExplosionParams Params;

	Params.NumParticles    = (int32)FRANDRANGE(30, 60);
	Params.MaxParticleSize = FRANDRANGE(3, 8) * 1.5f;
	Params.MinSpeed        =  90;
	Params.MaxSpeed        = 270;
	Params.bFade           = (FRAND >= 0.25f);
	Params.bCold           = true;
	Params.YoungColor[0]   = 
	Params.YoungColor[1]   = 
	Params.OldColor[0]     = 
	Params.OldColor[1]     = EColor::Purple;

	AddExplosionDebris(Params, Debris);

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		Params.NumParticles    = (int32)FRANDRANGE(20, 40);
		Params.MaxParticleSize = FRANDRANGE(8, 12);
		Params.MinSpeed        = 22;
		Params.MaxSpeed        = 67;
		Params.bFade           = (FRAND >= 0.25f);

		AddExplosionDebris(Params, Debris);
	}
}
