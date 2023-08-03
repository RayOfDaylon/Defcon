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

	PointValue           = SWARMER_VALUE;
	RadarColor           = C_ORANGE;
	Frequency            = 2.0f;
	SoundCountdown = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);
	
	HorzFrequency         = 2.0f * FRANDRANGE(0.5f, 1.5f);
	bCanBeInjured         = true;
	bIsCollisionInjurious = true;
	VerticalOffset        = (float)(FRAND * PI);
	TimeTargetWithinRange = 0.0f;
	FiringCountdown       = FRANDRANGE(1.0f, 3.0f);

	Orientation.Fwd.Set(1.0f, 0.0f);

	CreateSprite(Type);

	BboxRadius = GameObjectResources.Get(Type).Size / 2;
}


Defcon::CSwarmer::~CSwarmer()
{
}


#ifdef _DEBUG
const char* Defcon::CSwarmer::GetClassname() const
{
	static char* psz = "Swarmer";
	return psz;
};
#endif


void Defcon::CSwarmer::Move(float DeltaTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Move(DeltaTime);
	Inertia = Position;


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
				gpAudio->OutputSound(EAudioTrack::Swarmer);
				SoundCountdown = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);
			}


			if(SGN(Orientation.Fwd.x) == SGN(Direction.x))
			{
				ConsiderFiringBullet(DeltaTime);
			}
		}
	}


	Amplitude       = LERP(0.33f, 1.0f, PSIN(VerticalOffset + Age)) * 0.5f * ScreenSize.y;
	HalfwayAltitude = (float)(sin((VerticalOffset+Age) * 0.6f) * 50 + (0.5f * ScreenSize.y));

	CFPoint P;

	if(Age < 0.7f)
	{
		P.x = Position.x + .2f * Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * (FRAND * .05f + 0.25f);
	}
	else
	{
		P.x = Position.x + Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * (FRAND * .05f + 0.25f);
	}

	P.y = (float)sin(Frequency * (VerticalOffset + Age)) * Amplitude + HalfwayAltitude;

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

		const float XP = (float)GDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(2.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


void Defcon::CSwarmer::Explode(CGameObjectCollection& Debris)
{
	const auto ColorBase = EColor::Red;

	bMortal = true;
	Lifespan = 0.0f;
	OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* Flak = new CFlak;

		Flak->ColorbaseYoung = ColorBase;
		Flak->ColorbaseOld = ColorBase;
		Flak->bCold = true;
		Flak->LargestSize = 4;
		Flak->bFade = true;

		Flak->Position    = Position;
		Flak->Orientation = Orientation;

		double T = FRAND * TWO_PI;
		
		const CFPoint Direction((float)cos(T), (float)sin(T));

		// Debris has at least the object's momentum.
		Flak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		Flak->Orientation.Fwd *= FRANDRANGE(20, 32);
		Flak->Orientation.Fwd.MulAdd(Direction, FRANDRANGE(110, 140));

		Debris.Add(Flak);
	}
}
