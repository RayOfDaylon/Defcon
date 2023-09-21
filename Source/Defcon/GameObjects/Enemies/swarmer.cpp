// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	swarmer.cpp
	Swarmer type for Defcon game.
*/


#include "swarmer.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconGameInstance.h"


constexpr float SWARMER_SOUND_COUNTDOWN_MIN = 0.75f;
constexpr float SWARMER_SOUND_COUNTDOWN_MAX = 1.50f;


Defcon::CSwarmer::CSwarmer()
{
	ParentType = Type;
	Type       = EObjType::SWARMER;

	PointValue            = SWARMER_VALUE;
	RadarColor            = C_ORANGE;
	bCanBeInjured         = true;
	bIsCollisionInjurious = true;

	Frequency             = 2.0f;
	HorzFrequency         = 2.0f * FRANDRANGE(0.5f, 1.5f);
	VerticalOffset        = (float)(FRAND * PI);
	TimeTargetWithinRange = 0.0f;
	FiringCountdown       = FRANDRANGE(1.0f, 3.0f);
	SoundCountdown        = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);

	Orientation.Fwd.Set(SBRAND, 0.0f);

	CreateSprite(Type);

	BboxRadius = GGameObjectResources.Get(Type).Size / 2;
}


void Defcon::CSwarmer::OnFinishedCreating(const Daylon::FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	SetOriginalPosition(Position);

	if(Options.Has(TEXT("SmartbombInvulnerabilityCountdown")))
	{
		SmartbombInvulnerabilityCountdown = Options.GetReal(TEXT("SmartbombInvulnerabilityCountdown"));
	}
}


Defcon::CSwarmer::~CSwarmer()
{
}


void Defcon::CSwarmer::Tick(float DeltaTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Tick(DeltaTime);
	Inertia = Position;

	SmartbombInvulnerabilityCountdown -= DeltaTime;


	if(TargetPtr == nullptr)
	{
		TimeTargetWithinRange = 0.0f;
	}
	else
	{
		const bool bVis = IsOurPositionVisible();

		// Update target-within-range information.
		if(TimeTargetWithinRange > 0.0f)
		{
			// Target was in range; See if it left range.
			if(!bVis)
			{
				TimeTargetWithinRange = 0.0f;
			}
			else
			{
				TimeTargetWithinRange += DeltaTime;
			}
		}
		else
		{
			// Target was out of range; See if it entered range.
			if(bVis)
			{
				TimeTargetWithinRange = DeltaTime;

				//m_targetOffset.Set(
				//	LERP(-100, 100, FRAND), 
				//	LERP(50, 90, FRAND) * SGN(Position.y - TargetPtr->Position.y));
				//Frequency = LERP(6, 12, FRAND);
				//m_amp = LERP(.33f, .9f, FRAND);
			}
		}

		CFPoint Direction;
		const float Distance = GArena->ShortestDirection(Position, TargetPtr->Position, Direction);

		if(TimeTargetWithinRange > 0.75f)
		{
			if(Distance > ScreenSize.x * .4f)
			{
				Orientation.Fwd = Direction;
				Orientation.Fwd.y = 0;
				Orientation.Fwd.Normalize();
			}
		}

		if(TimeTargetWithinRange && Age > 1.0f)
		{
			SoundCountdown -= DeltaTime;

			if(SoundCountdown <= 0.0f)
			{
				GAudio->OutputSound(EAudioTrack::Swarmer);
				SoundCountdown = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);
			}


			if(SGN(Orientation.Fwd.x) == SGN(Direction.x))
			{
				ConsiderFiringBullet(DeltaTime);
			}
		}
	}


	Amplitude       = LERP(0.33f, 1.0f, PSIN(VerticalOffset + Age)) * 0.5f * ScreenSize.y;
	HalfwayAltitude = (sinf((VerticalOffset + Age) * 0.6f) * 50 + (0.5f * ScreenSize.y));

	CFPoint P;

	if(Age < 0.7f)
	{
		P.x = Position.x + .2f * Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * FRANDRANGE(0.25f, 0.3f);
	}
	else
	{
		P.x = Position.x + Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * FRANDRANGE(0.25f, 0.3f);
	}

	P.y = sinf(Frequency * (VerticalOffset + Age)) * Amplitude + HalfwayAltitude;

	Position = P;

	if(Age < 0.7f)
	{
		Position.y = LERP(OriginalPos.y, P.y, Age / 0.7f);
	}

	Inertia = Position - Inertia;
}


void Defcon::CSwarmer::ConsiderFiringBullet(float DeltaTime)
{
	if(!GArena->IsPointVisible(Position) || TargetPtr == nullptr)
	{
		return;
	}
		
	// Hold fire if target is below ground
	if(TargetPtr->Position.y < GArena->GetTerrainElev(TargetPtr->Position.x))
	{
		return;
	}

	FiringCountdown -= DeltaTime;

	if(FiringCountdown <= 0.0f)
	{
		(void) GArena->FireBullet(*this, Position, 1, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)GGameMatch->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(2.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


void Defcon::CSwarmer::Explode(CGameObjectCollection& Debris)
{
	FExplosionParams Params;

	Params.NumParticles    =  20;
	Params.MaxParticleSize =   4;
	Params.MinSpeed        =  90;
	Params.MaxSpeed        = 270;
	Params.bCold           = true;
	Params.YoungColor[0]   = 
	Params.YoungColor[1]   = 
	Params.OldColor[0]     = 
	Params.OldColor[1]     = EColor::Red;

	AddExplosionDebris(Params, Debris);
}
