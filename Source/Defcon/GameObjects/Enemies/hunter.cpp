// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	hunter.cpp
	Hunter type for Defcon game.
*/


#include "hunter.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconGameInstance.h"



Defcon::CHunter::CHunter()
{
	ParentType = Type;
	Type       = EObjType::HUNTER;

	PointValue            = HUNTER_VALUE;
	State                 = Lounging;
	RadarColor            = C_ORANGE;
	Brightness            = 1.0f;
	TimeTargetWithinRange = 0.0f;
	Amplitude             = FRANDRANGE(0.33f, 0.9f);
	
	const float Speed = Daylon::FRandRange(HUNTER_SPEED) * (BRAND ? 1 : -1);

	Orientation.Fwd.Set(Speed, 0);
	
	CreateSprite(Type);
	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::CHunter::~CHunter()
{
}


#ifdef _DEBUG
const char* Defcon::CHunter::GetClassname() const
{
	static char* psz = "Hunter";
	return psz;
};
#endif


void Defcon::CHunter::Tick(float DeltaTime)
{
	CEnemy::Tick(DeltaTime);


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

				TargetOffset.Set(FRANDRANGE(-100, 100), FRANDRANGE(50, 90) * SGN(Position.y - TargetPtr->Position.y));
				Frequency = FRANDRANGE(6, 12);
				Amplitude = FRANDRANGE(0.33f, 0.9f);
			}
		}
	}


	switch(State)
	{
		case Lounging:
			// Just float towards target unless the target
			// has become in range.
			if(TimeTargetWithinRange >= 0.33f)
			{
				State = Fighting;
			}

			else if(TargetPtr != nullptr)
			{
				GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);

				//Orientation.Fwd.Set(SGN(m_targetOffset.y), 0);
				Orientation.Fwd.y += (float)(Amplitude * sin(Age * Frequency));
				Position.MulAdd(Orientation.Fwd, DeltaTime * HUNTER_SPEED.Low() / 2);
			}
			break;


		case Evading:
		{
			if(TargetPtr != nullptr)
			{
				const float DeltaY = Position.y - TargetPtr->Position.y;

				if(ABS(DeltaY) > BboxRadius.y || SGN(TargetPtr->Orientation.Fwd.x) == SGN(Orientation.Fwd.x))
				{
					State = Fighting;
				}
				else
				{
					Orientation.Fwd.y = SGN(DeltaY) * 0.5f;
					//if(Orientation.Fwd.y == 0)
					//	Orientation.Fwd.y = SFRAND;
					CFPoint Pt;
					GArena->ShortestDirection(Position, TargetPtr->Position, Pt);
					Orientation.Fwd.x = (FRAND * 0.25f + 0.33f) * SGN(Pt.x);
					Position.MulAdd(Orientation.Fwd, DeltaTime * Daylon::Average(HUNTER_SPEED));
				}
			}
			
		}	
			break;


		case Fighting:
		{
			if(TimeTargetWithinRange == 0.0f)
			{
				State = Lounging;
			}
			else
			{
				float Distance = GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);
				const float DeltaY = Position.y - TargetPtr->Position.y;

				if(ABS(DeltaY) < BboxRadius.y && SGN(TargetPtr->Orientation.Fwd.x) != SGN(Orientation.Fwd.x))
				{
					// We can be hit by player; take evasive action.
					State = Evading;
				}
				else
				{
					const CFPoint Pt = TargetPtr->Position + TargetOffset;
					GArena->ShortestDirection(Position, Pt, Orientation.Fwd);

					Distance /= GArena->GetDisplayWidth();

					const float Speed = Distance >= 0.8f 
						? MAP(Distance, 0.8f, 1.0f, HUNTER_SPEED.High(), 0.0f)
						: RMAP(Distance, 0.0f, 0.8f, HUNTER_SPEED);

					Orientation.Fwd.y += (float)(Amplitude * sin(Age * Frequency));
					Position.MulAdd(Orientation.Fwd, DeltaTime * Speed);

					if(CanBeInjured() && TargetPtr->CanBeInjured() && Speed < 400)
					{
						FiringCountdown -= DeltaTime;

						ConsiderFiringBullet();
					}
				}
			}
		}
			break;
	} // switch(State)

	Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);

	// Constrain vertically.
	Position.y = CLAMP(Position.y, 0, GArena->GetHeight());

	Inertia = Position - Inertia;
}


void Defcon::CHunter::ConsiderFiringBullet()
{
	if(FiringCountdown <= 0.0f)
	{
		(void) GArena->FireBullet(*this, Position, (FRAND <= 0.85f) ? 2 : 3, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)GDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(1.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


Defcon::EColor Defcon::CHunter::GetExplosionColorBase() const
{
	return EColor::Yellow;
}


float Defcon::CHunter::GetExplosionMass() const
{
	return 1.5f;
}
