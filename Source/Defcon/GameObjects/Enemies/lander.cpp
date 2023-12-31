// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	lander.cpp
	Lander game object for Defcon game.

	rcg		jun 20/04	Fixed bug where ascended lander
						was able to keep itself and its
						human alive by resettng their
						lifespan to 0.1 every frame.
*/

#include "lander.h"

#include "DefconUtils.h"
#include "DefconLogging.h"
#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "GameObjects/human.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CLander::CLander()
{
	ParentType = Type;
	Type       = EObjType::LANDER;

	PointValue      = LANDER_VALUE;
	State           = EState::Descending;
	MaxSpeed        = FRANDRANGE(0.5f, 1.0f);

	{
		float N = NORM_(GGameMatch->GetScore(), 0, 75000) + FRANDRANGE(-0.05f, 0.05f);
		N = CLAMP(N, 0.0f, 1.0f);

		AscentSpeed  = Daylon::Lerp(LANDER_ASCENT_SPEED,  N);
		DescentSpeed = Daylon::Lerp(LANDER_DESCENT_SPEED, N);
	}

	RadarColor      = MakeColorFromComponents(255, 255, 0);
	HoverAltitude   = FRAND * 20 + 40;
	FiringCountdown = FRANDRANGE(1.0f, 3.0f);


	// With increasing probability over 10,000 pts,
	// and the nearest human is behind us, then 
	// change direction.
	// 0 - 10000    0.1
	// 60000+        0.9

	float ProbChaseHuman = 0.05f;

	const float Score = (float)GGameMatch->GetScore();

	if(Score > 10000 && Score <= 60000)
	{
		ProbChaseHuman = LERP(0.1f, 0.9f, (Score - 10000) / 50000);
	}
	else if(Score > 60000)
	{
		ProbChaseHuman = 0.95f;
	}

	bTryToAbduct = (FRAND <= ProbChaseHuman);


	CreateSprite(EObjType::LANDER);

	BboxRadius = GGameObjectResources.Get(Type).Size * (0.5f * 0.75f);//.Set(Info.Size.X * 0.5f * 0.75f, Info.Size.Y * 0.5f * 0.75f);
}


void Defcon::CLander::OnFinishedCreating(const FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	Orientation.Fwd.Set(FRANDRANGE(0.25f, 1.0f) * SBRAND, 0);

	Objects = &GArena->GetObjects();

	SetDoTryToAbduct(Defcon::GGameMatch->GetMission()->HumansInvolved());
}


Defcon::CLander::~CLander()
{
}


void Defcon::CLander::OnAboutToDie()
{
	if(Abductee != nullptr)
	{
 		Abductee->Notify(EMessage::CarrierKilled, this);
	}

	Abductee = nullptr;
}


void Defcon::CLander::Notify(Defcon::EMessage msg, void* pObj)
{
	switch(msg)
	{
		case Defcon::EMessage::HumanKilled:

			check(pObj != nullptr);

			if(pObj == AbductionTarget)
			{
				AbductionTarget = nullptr;
				State = EState::Hovering;
			}
			if(pObj == Abductee)
			{
				Abductee = nullptr;
				State = EState::Fighting;
			}

			break;


		case Defcon::EMessage::HumanTakenAboard:

			check(pObj != nullptr);

			if(pObj == AbductionTarget)
			{
				check(Abductee == nullptr);
				// Someone beat us to the punch.
				AbductionTarget = nullptr;
				State = EState::Hovering;
#ifdef _DEBUG
				OutputDebugString("Hunter: tracked human abducted by sibling\n");
#endif
			}
			break;

	}

	CEnemy::Notify(msg, pObj);
}


void Defcon::CLander::Tick(float DeltaTime)
{
	// Move towards target.

	CEnemy::Tick(DeltaTime);

	const float kHoverStartAlt = 100;

	Inertia = Position;



	ConsiderFiringBullet(DeltaTime);


	switch(State)
	{
		case EState::Descending:
			check(Abductee == nullptr);
			check(AbductionTarget == nullptr);

			if(Position.y < kHoverStartAlt)
				State = EState::Hovering;
			else
			{
				// Lower ourself.
				Orientation.Fwd.y = -1.0f;
				Position.MulAdd(Orientation.Fwd, DeltaTime * DescentSpeed);
			}
			break;


		case EState::Hovering:
		{
			// Move horizontally while staying over terrain.

			check(Abductee == nullptr);
			check(AbductionTarget == nullptr);

			const float Speed = MaxSpeed * 0.33f;

			CFPoint TargetPos; // where we want to move to.
				
			if(bTryToAbduct)
			{
				auto Human = GArena->FindNearestHuman(Position.x);

				if(Human != nullptr)
				{
					TargetPos = Human->Position;
					TargetPos.y += 27.0f; 
					// Don't center lander on top of human!
				}
				else
				{
					// If any humans are on the ground, lander switches to Fighting state too often
					// todo: we need to think about this just a little more. Was the game too boring
					// if landers were always hovering? Maybe they should switch to Fighting only 
					// if they've been hovering too long.
					//State = EState::Fighting;
					//break;
					TargetPos = Position;
					TargetPos += Orientation.Fwd;
				}
			}
			else
			{
				// Normal hovering; just move ahead.
				TargetPos = Position;
				TargetPos += Orientation.Fwd;
			}

			// Move towards target.
			check(Orientation.Fwd.Length() != 0.0f);

			PositionDelta(Orientation.Fwd, Position, TargetPos, ArenaSize.x);
			float dist = Orientation.Fwd.Length();
			Orientation.Fwd.Normalize();

			// Take terrain into account.
			const float fAltDelta = Position.y - GArena->GetTerrainElev(Position.x) - HoverAltitude;

			// If fAltDelta is +, we want to go down.
			// Go down slower than up.
			Orientation.Fwd.y = fAltDelta * (fAltDelta > 0 ? -0.01f : -0.03f);
			Orientation.Fwd.y = FMath::Min(Orientation.Fwd.y, 1.25f);
			Orientation.Fwd.y = FMath::Max(Orientation.Fwd.y, -1.25f);

			Position.MulAdd(Orientation.Fwd, DeltaTime * ScreenSize.x * Speed);

			// If we are near a human, and the odds 
			// say so or we've been around for more than 
			// 20 seconds, then switch to abduct mode.
			if(bTryToAbduct || Age > LANDER_MATURE || FRAND <= LANDER_ABDUCTODDS)
			{
				if(Defcon::GGameMatch->GetMission()->HumansInvolved())
				{
					AbductionTarget = GArena->FindNearestHuman(Position.x);

					if(AbductionTarget != nullptr)
					{
						State = EState::Acquiring;
					}
				}
			}
		}
			break;


		case EState::Acquiring: 
		{
			// If human exists, move towards it into docking
			// position. Otherwise, switch back to hovering.
			check(Abductee == nullptr);

			if(AbductionTarget == nullptr)
			{
				State = EState::Hovering;
			}
			else
			{
				// Human target exists. Attempt to dock to it.
				CFPoint target;
				target = AbductionTarget->Position;
				target.y += 27.0f; // Don't center lander with human!
				PositionDelta(Orientation.Fwd, Position, target, ArenaSize.x);
				float dist = Orientation.Fwd.Length();
				Orientation.Fwd.Normalize();
				float speed = MaxSpeed * 0.33f;
				const float motion = FMath::Min(DeltaTime * ScreenSize.x * speed, dist);
				Position.MulAdd(Orientation.Fwd, motion);

				// If we've docked, switch to ascent mode.

				CFPoint dd(target);

				if(dd.Distance(Position) < 2.0f)
				{
					State = EState::Ascending;
					Abductee = AbductionTarget;
					AbductionTarget = nullptr;

					// Ascend perfectly vertical half the time,
					// and otherwise, cut the horz. speed
					// by some random amount.
					bAscendStraight = BRAND;

					if(!bAscendStraight)
					{
						Orientation.Fwd.x *= FRAND;
					}
					// The human will notify our siblings 
					// that we've abducted him, thus if any of
					// them have been tracking him, they will
					// stop doing so.
					Abductee->Notify(Defcon::EMessage::TakenAboard, this);

					// Tell user only about the last human being abducted, otherwise the message log can pile up. 
					if(GGameMatch->GetHumans().Count() == 1)
					{
						const FString Str = TEXT("LAST HUMAN ABDUCTED -- MISSION FAILURE IMMINENT");
						GMessageMediator.TellUser(Str, MESSAGE_DURATION_IMPORTANT, EDisplayMessage::HumanBeingAbducted);
					}
					else
					{
						GMessageMediator.TellUser(TEXT("ABDUCTION IN PROGRESS"), MESSAGE_DURATION_IMPORTANT, EDisplayMessage::HumanBeingAbducted);
					}

					GMessageMediator.PlaySound(EAudioTrack::Human_abducted);
				}
			}
		}
			break;


		case EState::Ascending:
		{
			check(AbductionTarget == nullptr);

			if(Abductee == nullptr)
			{
				// The human we were taking was killed.
				// (Player is a lousy shot).
				State = EState::Fighting;
			}
			else
			{
				// Got the human, proceed to orbit.
				if(bAscendStraight)
				{
					Orientation.Fwd.x = 0.0f;
				}

				Orientation.Fwd.y = 1.0f;
				Position.MulAdd(Orientation.Fwd, DeltaTime * AscentSpeed);
			
				// Did we reach orbit?
				if(Abductee->Position.y > ArenaSize.y + Abductee->BboxRadius.y)
				{
					// Kill us and the human.
					// (arcade mismatch) todo: should turn lander into a mutant.
					State = EState::Ascended;
					bMortal = true;
					MarkAsDead();
					Abductee->bMortal = true;
					Abductee->MarkAsDead();
					GArena->OnHumansChanged();
				}
			}
		}
			break;


		case EState::Fighting:
		{
			check(Abductee == nullptr);
			check(AbductionTarget == nullptr);

			if(bTryToAbduct)
			{
				if(GArena->FindNearestHuman(Position.x) != nullptr)
				{
					State = EState::Hovering;
					break;
				}
			}

			const float Speed = MaxSpeed * 0.33f;

			if(TargetPtr == nullptr)
			{
				// No target, just coast.
				Orientation.Fwd.y = 0;
			}
			else
			{
				// Target present, move towards it.
				GArena->ShortestDirection(Position, TargetPtr->Position, Orientation.Fwd);
				//Orientation.Fwd = pTarget->Position;
				//float dist = Orientation.Fwd.Distance(Position);
				//Orientation.Fwd -= Position;
				//if(dist > ArenaSize.x / 2)
				//	Orientation.Fwd *= -1;
				//Orientation.Fwd.Normalize();
			}

#if 0
			CFPoint fpos(Position);
			fpos.MulAdd(Orientation.Fwd, DeltaTime * ScreenSize.x * Speed);

			// Avoid moving too close to other hunters.
			IGameObject* pObj = Objects->GetFirst();
			while(pObj != nullptr)
			{
				int32 id = pObj->GetID();
				if(id != 1 && id != 6) // not the player or human
				{
					float dist1 = Position.distance(pObj->Position);
					float dist2 = fpos.Distance(pObj->Position);
					if(dist1 < m_personalSpace)
					{
						if(dist2 < dist1)
						{
							// Reverse course at reduced speed.
							Speed *= 0.9f;
							Orientation.Fwd *= -1;
							Orientation.Fwd.y *= 2.0f;
							break;
						}
						else
						{
							Speed *= 0.7f;
							Orientation.Fwd.y *= 1.0f;
						}
					}
				}
				pObj = pObj->GetNext();
			}
#endif

			Position.MulAdd(Orientation.Fwd, DeltaTime * ScreenSize.x * Speed);
		}
			break;
	} // switch(state)

	Inertia = Position - Inertia;
}


void Defcon::CLander::ConsiderFiringBullet(float DeltaTime)
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
		(void) GArena->FireBullet(*this, Position, (FRAND <= 0.85f) ? 2 : 3, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)GGameMatch->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(3.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 1.0f);
	}
}


Defcon::EColor Defcon::CLander::GetExplosionColorBase() const
{
	//return EColor::LightYellow;
 	//return EColor::Yellow;
	return EColor::Green;
}
