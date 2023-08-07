// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	guppy.cpp
	Yllabian Guppy type for Defcon game.
*/

#include "guppy.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconGameInstance.h"



Defcon::CGuppy::CGuppy()
{
	ParentType = Type;
	Type = EObjType::GUPPY;
	PointValue = GUPPY_VALUE;
	State = Lounging;
	
	float speed = FRANDRANGE(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);
	if(FRAND > 0.5f) 
		speed *= -1;
	Orientation.Fwd.Set(speed, 0);
	//m_personalSpace = FRAND * 60 + 20;
	RadarColor = C_ORANGE;

	AnimSpeed = FRAND * 0.6f + 0.6f;
	Brightness = 1.0f;
	TimeTargetWithinRange = 0.0f;
	Amplitude = FRANDRANGE(0.25f, 0.4f);
	FiringCountdown = 1.0f;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2 - 1);
}


#ifdef _DEBUG
const char* Defcon::CGuppy::GetClassname() const
{
	static char* psz = "Guppy";
	return psz;
};
#endif


void Defcon::CGuppy::Move(float DeltaTime)
{
	// Move towards target.

	CEnemy::Move(DeltaTime);

	Inertia = Position;
	
	TargetPtr;

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
				TimeTargetWithinRange = 0.0f;
			else
				TimeTargetWithinRange += DeltaTime;
		}
		else
		{
			// Target was out of range; See if it entered range.
			if(bVis)
			{
				TimeTargetWithinRange = DeltaTime;

				TargetOffset.Set(
					FRANDRANGE(-100, 100), 
					FRANDRANGE(50, 90) * SGN(Position.y - TargetPtr->Position.y));

				Frequency = FRANDRANGE(6, 12);
				Amplitude = FRANDRANGE(0.33f, 0.9f);
			}
		}
	}


	switch(State)
	{
		case Lounging:

			// Just float towards target unless the target has come in range.
			if(TimeTargetWithinRange >= 0.33f)
			{
				State = Fighting;
			}

			else if(TargetPtr != nullptr)
			{
				CFPoint pt;
				GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);

				//Orientation.Fwd.Set(SGN(m_targetOffset.y), 0);
				Orientation.Fwd.y += (float)(Amplitude * sin(Age * Frequency));
				Position.MulAdd(Orientation.Fwd, DeltaTime * GUPPY_SPEEDMIN/2);
			}
			break;


		case Evading:
		{
			if(TargetPtr != nullptr)
			{
				float vd = Position.y - TargetPtr->Position.y;
				if(ABS(vd) > BboxRadius.y || 
					SGN(TargetPtr->Orientation.Fwd.x) == 
					SGN(Orientation.Fwd.x))
					State = Fighting;
				else
				{
					Orientation.Fwd.y = SGN(vd)*0.5f;
					//if(Orientation.Fwd.y == 0)
					//	Orientation.Fwd.y = SFRAND;
					CFPoint pt;
					GArena->ShortestDirection(Position, TargetPtr->Position, pt);
					Orientation.Fwd.x = (FRAND * 0.25f + 0.33f) * SGN(pt.x);
					Position.MulAdd(Orientation.Fwd, DeltaTime * AVG(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX));
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
				float dist = GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);
				float vd = Position.y - TargetPtr->Position.y;
				if(ABS(vd) < BboxRadius.y && SGN(TargetPtr->Orientation.Fwd.x) != SGN(Orientation.Fwd.x))
				{
					// We can be hit by player. 
					// Take evasive action.
					State = Evading;
				}
				else
				{
					CFPoint pt = TargetPtr->Position + TargetOffset;
					GArena->ShortestDirection(Position, pt, Orientation.Fwd);

					dist /= GArena->GetDisplayWidth();

					const float speed = dist >= 0.8f 
						? MAP(dist, 0.8f, 1.0f, GUPPY_SPEEDMAX, 0.0f)
						: MAP(dist, 0.0f, 0.8f, GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);
					
					Orientation.Fwd.y += (float)(Amplitude * sin(Age * Frequency));
					Position.MulAdd(Orientation.Fwd, DeltaTime * speed);

					if(IsOurPositionVisible() && CanBeInjured() && TargetPtr->CanBeInjured() && speed < 400)
					{
						FiringCountdown -= DeltaTime;

						ConsiderFiringBullet();
					}

				}
			}
		}
			break;
	} // switch(state)

	// Constrain vertically.
	Position.y = CLAMP(Position.y, 0, GArena->GetHeight());

	Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);

	Inertia = Position - Inertia;
}


void Defcon::CGuppy::ConsiderFiringBullet()
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


Defcon::EColor Defcon::CGuppy::GetExplosionColorBase() const
{
	return EColor::Magenta;
}


float Defcon::CGuppy::GetExplosionMass() const
{
	return 0.3f;
}


void Defcon::CGuppy::Explode(CGameObjectCollection& Debris)
{
	FExplosionParams Params;

	Params.bCold           = true;
	Params.bFade           = true;
	Params.MaxParticleSize =   6;
	Params.NumParticles    =  20;
	Params.MinSpeed        = 110;
	Params.MaxSpeed        = 140;
	Params.OldColor[0]     = EColor::Magenta;
	Params.OldColor[1]     = EColor::Orange;
	Params.YoungColor[0]   = EColor::Magenta;
	Params.YoungColor[1]   = EColor::Red;

	AddExplosionDebris(Params, Debris);
}
