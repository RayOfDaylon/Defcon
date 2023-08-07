// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	firebomber.cpp
	Firebomber enemy type for Defcon game.

	Firebomber movement is composed of travel for a short period
	along a straight line (a diagonal one), after which the travel 
	direction is reversed horizontally but may or may not reverse
	vertically. The distance is always random.
*/


#include "firebomber.h"

#include "DefconUtils.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"



// -------------------------------------------------

Defcon::IFirebomber::IFirebomber()
{
	ParentType = Type;
	Type = EObjType::FIREBOMBER;
	PointValue = FIREBOMBER_VALUE;

	RadarColor = C_WHITE;
	
	AnimSpeed = FRAND * 0.05f + 0.12f;

	FiringCountdown = 2.0f;
	TravelCountdown = 1.0f;

	Orientation.Fwd.Set(SBRAND, SBRAND);
	OurInertia = Orientation.Fwd * Daylon::FRandRange(FIREBOMBER_SPEED_MIN, FIREBOMBER_SPEED_MAX);

	const auto& Info = GameObjectResources.Get(EObjType::FIREBOMBER_TRUE);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::IFirebomber::~IFirebomber()
{
}

#ifdef _DEBUG
const char* Defcon::IFirebomber::GetClassname() const
{
	static char* psz = "Firebomber";
	return psz;
};
#endif


void Defcon::IFirebomber::Move(float fTime)
{
	// Just float around drifting horizontally.

	Super::Move(fTime);
	Inertia = Position;

	TravelCountdown -= fTime;

	if(TravelCountdown <= 0.0f)
	{
		// We've finished traveling, so define a new direction and travel length.
		Orientation.Fwd.Set(SBRAND, SBRAND);
		OurInertia      = Orientation.Fwd * Daylon::FRandRange(FIREBOMBER_SPEED_MIN, FIREBOMBER_SPEED_MAX);

		if(IRAND(3) == 1)
		{
			OurInertia *= 0.5f;
		}

		TravelCountdown = Daylon::FRandRange(FIREBOMBER_TRAVEL_TIME_MIN, FIREBOMBER_TRAVEL_TIME_MAX);
	}

	Position += OurInertia * fTime;


	WRAP(Position.y, 0, ScreenSize.y);

	if(IsOurPositionVisible() && TargetPtr != nullptr)
	{
		FiringCountdown -= fTime;
	}

	Inertia = Position - Inertia;
}


void Defcon::IFirebomber::Explode(CGameObjectCollection& debris)
{
	// Explode in a thick symmetrical pattern.

	float fBrightBase;
	CreateExplosionFireball(debris, fBrightBase);

	int32 A, I;
	const float Off = SFRAND * 0.2f;
	const float Off2 = SFRAND * 0.2f;

	for(A = 0; A < 8; A++)
	{
		for(I = 0; I < 10; I++)
		{
			CFlak* pFlak = new CFlak;
			pFlak->ColorbaseYoung = BRAND ? EColor::Gray : EColor::Yellow;

			float Largest = FRANDRANGE(5, 11);
			pFlak->LargestSize = MAP(I, 0, 9, Largest, 4);
			pFlak->bFade = true;

			pFlak->Position = Position;

			float Angle = MAP(A, 0, 7, 0, 5.5f);
			Angle += Off + SFRAND * 0.05f;
			pFlak->Orientation.Fwd.Set(sinf(Angle), cosf(Angle));
			
			pFlak->Orientation.Fwd *= (SFRAND * 15 + 30) * (I + 2);
			pFlak->Orientation.Fwd += Inertia;

			debris.Add(pFlak);
		}
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		for(A = 0; A < 8; A++)
		{
			for(I = 0; I < 10; I++)
			{
				CFlak* pFlak = new CFlak;
				pFlak->ColorbaseYoung = BRAND ? EColor::Gray : EColor::Yellow;

				float Largest = FRAND * 6 + 5;
				pFlak->LargestSize = MAP(I, 0, 9, Largest, 4);
				pFlak->bFade = true;

				pFlak->Position = Position;

				float Angle = MAP(A, 0, 7, 0, 5.5f);
				Angle += Off2 + SFRAND * 0.05f;
				pFlak->Orientation.Fwd.Set(sinf(Angle), cosf(Angle));
				
				pFlak->Orientation.Fwd *= (SFRAND * 5 + 6) * (I + 2);
				pFlak->Orientation.Fwd += Inertia;

				debris.Add(pFlak);
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------

Defcon::CFirebomber::CFirebomber()
{
	ParentType = Type;
	Type       = EObjType::FIREBOMBER_TRUE;

	CreateSprite(Type);
}


Defcon::CFirebomber::~CFirebomber()
{
}


void Defcon::CFirebomber::Move(float fTime)
{
	Super::Move(fTime);

	if(!(CanBeInjured() && GArena->GetPlayerShip().IsAlive() && IsOurPositionVisible()))
	{
		return;
	}

	if(FiringCountdown <= 0.0f)
	{
		GArena->CreateEnemy(EObjType::FIREBALL, Position, 0.0f, EObjectCreationFlags::EnemyPart);

		// The time to fire goes down as the player XP increases.

		float XP = (float)GDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(2.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


// ------------------------------------------------------------------------------------------------------

Defcon::CWeakFirebomber::~CWeakFirebomber()
{
}


Defcon::CWeakFirebomber::CWeakFirebomber()
{
	ParentType = Type;
	Type       = EObjType::FIREBOMBER_WEAK;

	CreateSprite(Type);
}


void Defcon::CWeakFirebomber::Move(float fTime)
{
	Super::Move(fTime);

	if(FiringCountdown <= 0.0f)
	{
		GArena->FireBullet(*this, Position, 1, 1);

		// The time to fire goes down as the player XP increases.

		float XP = (float)GDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(2.0f, 0.1f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}
