// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	pod.cpp
	Pod enemy type for Defcon game.
*/

#include "pod.h"


#include "Common/util_color.h"
#include "Common/variant.h"
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


void Defcon::CPod::OnFinishedCreating(const FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	check(Options.Num() >= 2);

	Orientation.Fwd.x = Options.GetReal(EMetadataKey::OrientationX);
	SetSpeed(Options.GetReal(EMetadataKey::SpeedX));
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
	if(DeathStyle != EDeathStyle::IncludeCrew)
	{
		// Dying normally; release swarmers.

		FMetadata Options;
		FMetadata* OptionsPtr = nullptr;

		if(KillerType == EObjType::SMARTBOMB)
		{
			// We were destroyed by a smart bomb but releasing swarmers, so tag each swarmer
			// with a short duration of smart bomb invulnerability. 
			// todo: this won't work if the player fires a second smartbomb quickly after the first
			// in an attempt to destroy the swarmers, or just as insurance when he has lots of bombs.
			// For that, we need to track and include a smartbomb ID.

			Options.Add(EMetadataKey::SmartbombID, KillerID);
			OptionsPtr = &Options;
		}

		for(int32 I = 0; I < SWARMERS_PER_POD; I++)
		{
			GArena->SpawnGameObject(EObjType::SWARMER, GetType(), Position, 0.0f, EObjectCreationFlags::EnemyPart, OptionsPtr);
		}
	}
}


void Defcon::CPod::Explode(CGameObjectCollection& Debris)
{
	// Pods explode normally (but deep purple or red)
	// but if collided with perfectly, even more so because 
	// all the swarmers are biting it.

	FExplosionParams Params;

	if(DeathStyle == EDeathStyle::Normal)
	{
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
	else
	{
		// "Fat" explosion because we are taking our swarmers out with us.

		Params.NumParticles    = (int32)FRANDRANGE(90, 120);
		Params.MaxParticleSize = FRANDRANGE(6, 16) * 1.5f;
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
			Params.NumParticles    = (int32)FRANDRANGE(40, 80);
			Params.MaxParticleSize = FRANDRANGE(8, 16);
			Params.MinSpeed        = 22;
			Params.MaxSpeed        = 67;
			Params.bFade           = (FRAND >= 0.25f);

			AddExplosionDebris(Params, Debris);
		}

		// Include points for swarmers.
		GGameMatch->AdvanceScore(SWARMER_VALUE * SWARMERS_PER_POD);
	}
}
