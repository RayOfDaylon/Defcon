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
	Type				= EObjType::LANDER;
	PointValue		= LANDER_VALUE;
	m_eState			= State::descending;
	TargetPtr			= nullptr;
	m_pHuman			= nullptr;
	m_pTrackedHuman		= nullptr;
	float speed			= (FRAND * .75f + 0.25f) * (FRAND > 0.5f ? -1 : 1);
	m_personalSpace		= FRAND * 60 + 20;
	m_maxSpeed			= FRAND * 0.5f + 0.5f;
	RadarColor		= MakeColorFromComponents(255, 255, 0);
	m_fSpeed			= FRAND + 1.0f;
	m_fHoverAltitude	= FRAND * 20 + 40;
	AnimSpeed		= FRAND * 0.66f + 0.33f;

	Orientation.Fwd.Set(speed, 0);
	//BboxRadius.Set(11, 11); // todo: s/b based on sprite size?

	// With increasing probability over 10,000 pts,
	// and the nearest human is behind us, then 
	// change direction.
	// 0 - 10000    0.1
	// 60000+        0.9
	float fProbChaseHuman = 0.05f;

	const float score = (float)gDefconGameInstance->GetScore();

	if(score > 10000 && score <= 60000)
	{
		fProbChaseHuman = LERP(0.1f, 0.9f, (score-10000)/50000);
	}
	else if(score > 60000)
	{
		fProbChaseHuman = 0.95f;
	}

	m_bChaseNearestHuman = (FRAND <= fProbChaseHuman);

	CreateSprite(EObjType::LANDER);

	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X * 0.5f * 0.75f, Info.Size.Y * 0.5f * 0.75f);
}


Defcon::CLander::~CLander()
{
}


void Defcon::CLander::OnAboutToDie()
{
	if(m_pHuman != nullptr)
	{
 		m_pHuman->Notify(EMessage::CarrierKilled, this);
	}

	m_pHuman = nullptr;
}


#ifdef _DEBUG
const char* Defcon::CLander::GetClassname() const
{
	static char* psz = "Lander";
	return psz;
};
#endif


void Defcon::CLander::Notify(Defcon::EMessage msg, void* pObj)
{
	switch(msg)
	{
		case Defcon::EMessage::HumanKilled:

			check(pObj != nullptr);

			if(pObj == m_pTrackedHuman)
			{
				m_pTrackedHuman = nullptr;
				m_eState = State::hovering;
			}
			if(pObj == m_pHuman)
			{
				m_pHuman = nullptr;
				m_eState = State::fighting;
			}

			break;


		case Defcon::EMessage::HumanTakenAboard:

			if(pObj == m_pTrackedHuman)
			{
				check(m_pHuman == nullptr);
				// Someone beat us to the punch.
				m_pTrackedHuman = nullptr;
				m_eState = State::hovering;
#ifdef _DEBUG
				OutputDebugString("Hunter: tracked human abducted by sibling\n");
#endif
			}
			break;

	}

	CEnemy::Notify(msg, pObj);
}


void Defcon::CLander::Move(float fTime)
{
	// Move towards target.

	CEnemy::Move(fTime);

	const float kHoverStartAlt = 100;

	Inertia = Position;

	// if we are on screen then
	//   if the dice roll well enough
	//     fire bullet

	// todo: odds of firing a bullet need to be time based, not frame based.
	// e.g. some deviation from a base bullets-per-second frequency.
	if(FRAND <= 0.005f 
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& IsOurPositionVisible())
		gpArena->FireBullet(*this, Position, 1, 1);


	switch(m_eState)
	{
		case State::descending:
			check(m_pHuman == nullptr);
			check(m_pTrackedHuman == nullptr);

			if(Position.y < kHoverStartAlt)
				m_eState = State::hovering;
			else
			{
				// Lower ourself.
				Orientation.Fwd.y = -1.0f;
				Position.MulAdd(Orientation.Fwd, fTime * m_fSpeed * LANDER_DESCENT_SPEED);
			}
			break;


		case State::hovering:
		{
			// Move horizontally while staying over terrain.

			check(m_pHuman == nullptr);
			check(m_pTrackedHuman == nullptr);

			float speed = m_maxSpeed * 0.33f;
			//check(m_fnTerrainEval != nullptr);


			CFPoint target; // where we want to move to.
				
			if(m_bChaseNearestHuman)
			{
				// todo: why aren't we using m_fnHumanFinder all the time?
				//Defcon::CHuman* pHuman = gpGame->FindHuman(Position);
				Defcon::CHuman* pHuman = (CHuman*)gpArena->FindHuman(Position.x);
				if(pHuman != nullptr)
				{
					target = pHuman->Position;
					target.y += 27.0f; 
					// Don't center lander on top of human!
				}
				else
				{
					m_eState = State::fighting;
					break;
				}
			}
			else
			{
				// Normal hovering; just move ahead.
				target = Position;
				target += Orientation.Fwd;
			}

			// Move towards target.
			check(Orientation.Fwd.Length() != 0.0f);

			PositionDelta(Orientation.Fwd, Position, target, ArenaSize.x);
			float dist = Orientation.Fwd.Length();
			Orientation.Fwd.Normalize();

			// Take terrain into account.
			//float fAltDelta = Position.y - m_fnTerrainEval(Position.x, m_pvUserTerrainEval);
			float fAltDelta = Position.y - gpArena->GetTerrainElev(Position.x);
			fAltDelta -= m_fHoverAltitude;
			// If fAltDelta is +, we want to go down.
			// Go down slower than up.
			Orientation.Fwd.y = fAltDelta * (fAltDelta > 0 ? -0.01f : -0.03f);
			Orientation.Fwd.y = FMath::Min(Orientation.Fwd.y, 1.25f);
			Orientation.Fwd.y = FMath::Max(Orientation.Fwd.y, -1.25f);

			Position.MulAdd(Orientation.Fwd, fTime * ScreenSize.x * speed);

			// If we are near a human, and the odds 
			// say so or we've been around for more than 
			// 20 seconds, then switch to abduct mode.
			if(m_bChaseNearestHuman || Age > LANDER_MATURE || FRAND <= LANDER_ABDUCTODDS)
			{
				if(gDefconGameInstance->GetMission()->HumansInvolved())
				{
					//m_pTrackedHuman = m_fnHumanFinder(Position.x, m_pvUserTerrainEval);
					m_pTrackedHuman = (CHuman*)gpArena->FindHuman(Position.x);
					if(m_pTrackedHuman != nullptr)
						m_eState = State::acquiring;
				}
			}
		}
			break;


		case State::acquiring: 
		{
			// If human exists, move towards it into docking
			// position. Otherwise, switch back to hovering.
			check(m_pHuman == nullptr);

			if(m_pTrackedHuman == nullptr)
			{
				m_eState = State::hovering;
			}
			else
			{
				// Human target exists. Attempt to dock to it.
				CFPoint target;
				target = m_pTrackedHuman->Position;
				target.y += 27.0f; // Don't center lander with human!
				PositionDelta(Orientation.Fwd, Position, target, ArenaSize.x);
				float dist = Orientation.Fwd.Length();
				Orientation.Fwd.Normalize();
				float speed = m_maxSpeed * 0.33f;
				float motion = FMath::Min(fTime * ScreenSize.x * speed, dist);
				Position.MulAdd(Orientation.Fwd, /*fTime * 80.0f*/motion);

				// If we've docked, switch to ascent mode.

				CFPoint dd(target);

				if(dd.Distance(Position) < 2.0f)
				{
					m_eState = State::ascending;
					m_pHuman = m_pTrackedHuman;
					m_pTrackedHuman = nullptr;

					// Ascend perfectly vertical half the time,
					// and otherwise, cut the horz. speed
					// by some random amount.
					m_bAscendStraight = (FRAND >= 0.5f);
					if(!m_bAscendStraight)
					{
						Orientation.Fwd.x *= FRAND;
					}
					// The human will notify our siblings 
					// that we've abducted him, thus if any of
					// them have been tracking him, they will
					// stop doing so.
					m_pHuman->Notify(Defcon::EMessage::TakenAboard, this);
					gpAudio->OutputSound(EAudioTrack::Human_abducted);
				}
			}
		}
			break;


		case State::ascending:
		{
			check(m_pTrackedHuman == nullptr);

			if(m_pHuman == nullptr)
			{
				// The human we were taking was killed.
				// (Player is a lousy shot).
				m_eState = State::fighting;
			}
			else
			{
				// Got the human, proceed to orbit.
				if(m_bAscendStraight)
					Orientation.Fwd.x = 0.0f;

				Orientation.Fwd.y = 1.0f;
				Position.MulAdd(Orientation.Fwd, fTime * LANDER_ASCENTRATE);
			
				// Did we reach orbit?
				if(m_pHuman->Position.y > ArenaSize.y + m_pHuman->BboxRadius.y)
				{
					// Kill us and the human.
					this->m_eState = State::ascended;
					this->bMortal = true;
					this->MarkAsDead();
					m_pHuman->bMortal = true;
					m_pHuman->MarkAsDead();
				}
			}
		}
			break;


		case State::fighting:
		{
			check(m_pHuman == nullptr);
			check(m_pTrackedHuman == nullptr);

			if(m_bChaseNearestHuman)
			{
				//Defcon::CHuman* pHuman = gpGame->FindHuman(Position);
				Defcon::CHuman* pHuman = (CHuman*) gpArena->FindHuman(Position.x);
				if(pHuman != nullptr)
				{
					m_eState = State::hovering;
					break;
				}
			}

			float speed = m_maxSpeed * 0.33f;

			IGameObject* pTarget = TargetPtr;
			if(pTarget == nullptr)
			{
				// No target, just coast.
				Orientation.Fwd.y = 0;
			}
			else
			{
				// Target present, move towards it.
				gpArena->Direction(Position, pTarget->Position, Orientation.Fwd);
				//Orientation.Fwd = pTarget->Position;
				//float dist = Orientation.Fwd.Distance(Position);
				//Orientation.Fwd -= Position;
				//if(dist > ArenaSize.x / 2)
				//	Orientation.Fwd *= -1;
				//Orientation.Fwd.Normalize();
			}

			CFPoint fpos(Position);
			fpos.MulAdd(Orientation.Fwd, fTime * ScreenSize.x * speed);

#if 0
			// Avoid moving too close to other hunters.
			IGameObject* pObj = Objects->GetFirst();
			while(pObj != nullptr)
			{
				int id = pObj->GetID();
				if(id != 1 && id != 6) // not the player or human
				{
					float dist1 = Position.distance(pObj->Position);
					float dist2 = fpos.Distance(pObj->Position);
					if(dist1 < m_personalSpace)
					{
						if(dist2 < dist1)
						{
							// Reverse course at reduced speed.
							speed *= 0.9f;
							Orientation.Fwd *= -1;
							Orientation.Fwd.y *= 2.0f;
							break;
						}
						else
						{
							speed *= 0.7f;
							Orientation.Fwd.y *= 1.0f;
						}
					}
				}
				pObj = pObj->GetNext();
			}
#endif

			Position.MulAdd(Orientation.Fwd, fTime * ScreenSize.x * speed);
		}
			break;
	} // switch(state)

	Inertia = Position - Inertia;

	//m_fNow = GameTime();
}


void Defcon::CLander::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


Defcon::EColor Defcon::CLander::GetExplosionColorBase() const
{
	//return EColor::LightYellow;
 	//return EColor::Yellow;
	return EColor::Green;
}
