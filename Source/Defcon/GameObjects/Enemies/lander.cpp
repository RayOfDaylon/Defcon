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
	m_type				= ObjType::LANDER;
	m_pointValue		= LANDER_VALUE;
	m_eState			= State::descending;
	m_pTarget			= nullptr;
	m_pHuman			= nullptr;
	m_pTrackedHuman		= nullptr;
	float speed			= (FRAND * .75f + 0.25f) * (FRAND > 0.5f ? -1 : 1);
	m_personalSpace		= FRAND * 60 + 20;
	m_maxSpeed			= FRAND * 0.5f + 0.5f;
	m_smallColor		= MakeColorFromComponents(255, 255, 0);
	m_fSpeed			= FRAND + 1.0f;
	m_fHoverAltitude	= FRAND * 20 + 40;
	//m_fnTerrainEval		= nullptr;
	m_fAnimSpeed		= FRAND * 0.66f + 0.33f;

	m_orient.fwd.Set(speed, 0);
	//m_bboxrad.Set(11, 11); // todo: s/b based on sprite size?

	// With increasing probability over 10,000 pts,
	// and the nearest human is behind us, then 
	// change direction.
	// 0 - 10000    0.1
	// 60000+        0.9
	float fProbChaseHuman = 0.05f;
	//const float score = (float)UDefconUtils::GetGameInstance(gpArena)->GetScore();
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

	CreateSprite(ObjType::LANDER);

	const auto& Info = GameObjectResources.Get(m_type);
	m_bboxrad.Set(Info.Size.X * 0.5f * 0.75f, Info.Size.Y * 0.5f * 0.75f);
}


Defcon::CLander::~CLander()
{
}


void Defcon::CLander::OnAboutToDie()
{
	if(m_pHuman != nullptr)
	{
 		m_pHuman->Notify(Message::carrier_killed, this);
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


void Defcon::CLander::Notify(Defcon::Message msg, void* pObj)
{
	switch(msg)
	{
		case Defcon::Message::human_killed:

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


		case Defcon::Message::human_takenaboard:

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

	m_inertia = m_pos;

	// if we are on screen then
	//   if the dice roll well enough
	//     fire bullet

	// todo: odds of firing a bullet need to be time based, not frame based.
	// e.g. some deviation from a base bullets-per-second frequency.
	if(FRAND <= 0.005f 
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(m_pos))
		gpArena->FireBullet(*this, m_pos, 1, 1);


	switch(m_eState)
	{
		case State::descending:
			check(m_pHuman == nullptr);
			check(m_pTrackedHuman == nullptr);

			if(m_pos.y < kHoverStartAlt)
				m_eState = State::hovering;
			else
			{
				// Lower ourself.
				m_orient.fwd.y = -1.0f;
				m_pos.MulAdd(m_orient.fwd, fTime * m_fSpeed * LANDER_DESCENT_SPEED);
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
				//Defcon::CHuman* pHuman = gpGame->FindHuman(m_pos);
				Defcon::CHuman* pHuman = (CHuman*)gpArena->FindHuman(m_pos.x);
				if(pHuman != nullptr)
				{
					target = pHuman->m_pos;
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
				target = m_pos;
				target += m_orient.fwd;
			}

			// Move towards target.
			check(m_orient.fwd.Length() != 0.0f);

			pos_delta(m_orient.fwd, m_pos, target, m_arenasize.x);
			float dist = m_orient.fwd.Length();
			m_orient.fwd.Normalize();

			// Take terrain into account.
			//float fAltDelta = m_pos.y - m_fnTerrainEval(m_pos.x, m_pvUserTerrainEval);
			float fAltDelta = m_pos.y - gpArena->GetTerrainElev(m_pos.x);
			fAltDelta -= m_fHoverAltitude;
			// If fAltDelta is +, we want to go down.
			// Go down slower than up.
			m_orient.fwd.y = fAltDelta * (fAltDelta > 0 ? -0.01f : -0.03f);
			m_orient.fwd.y = FMath::Min(m_orient.fwd.y, 1.25f);
			m_orient.fwd.y = FMath::Max(m_orient.fwd.y, -1.25f);

			m_pos.MulAdd(m_orient.fwd, fTime * m_screensize.x * speed);

			// If we are near a human, and the odds 
			// say so or we've been around for more than 
			// 20 seconds, then switch to abduct mode.
			if(m_bChaseNearestHuman || m_fAge > LANDER_MATURE || FRAND <= LANDER_ABDUCTODDS)
			{
				if(gDefconGameInstance->GetMission()->HumansInvolved())
				{
					//m_pTrackedHuman = m_fnHumanFinder(m_pos.x, m_pvUserTerrainEval);
					m_pTrackedHuman = (CHuman*)gpArena->FindHuman(m_pos.x);
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
				target = m_pTrackedHuman->m_pos;
				target.y += 27.0f; // Don't center lander with human!
				pos_delta(m_orient.fwd, m_pos, target, m_arenasize.x);
				float dist = m_orient.fwd.Length();
				m_orient.fwd.Normalize();
				float speed = m_maxSpeed * 0.33f;
				float motion = FMath::Min(fTime * m_screensize.x * speed, dist);
				m_pos.MulAdd(m_orient.fwd, /*fTime * 80.0f*/motion);

				// If we've docked, switch to ascent mode.

				CFPoint dd(target);

				if(dd.Distance(m_pos) < 2.0f)
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
						m_orient.fwd.x *= FRAND;
					}
					// The human will notify our siblings 
					// that we've abducted him, thus if any of
					// them have been tracking him, they will
					// stop doing so.
					m_pHuman->Notify(Defcon::Message::takenaboard, this);
					gpAudio->OutputSound(snd_human_abducted);
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
					m_orient.fwd.x = 0.0f;

				m_orient.fwd.y = 1.0f;
				m_pos.MulAdd(m_orient.fwd, fTime * LANDER_ASCENTRATE);
			
				// Did we reach orbit?
				if(m_pHuman->m_pos.y > m_arenasize.y + m_pHuman->m_bboxrad.y)
				{
					// Kill us and the human.
					this->m_eState = State::ascended;
					this->m_bMortal = true;
					this->MarkAsDead();
					m_pHuman->m_bMortal = true;
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
				//Defcon::CHuman* pHuman = gpGame->FindHuman(m_pos);
				Defcon::CHuman* pHuman = (CHuman*) gpArena->FindHuman(m_pos.x);
				if(pHuman != nullptr)
				{
					m_eState = State::hovering;
					break;
				}
			}

			float speed = m_maxSpeed * 0.33f;

			IGameObject* pTarget = m_pTarget;
			if(pTarget == nullptr)
			{
				// No target, just coast.
				m_orient.fwd.y = 0;
			}
			else
			{
				// Target present, move towards it.
				gpArena->Direction(m_pos, pTarget->m_pos, m_orient.fwd);
				//m_orient.fwd = pTarget->m_pos;
				//float dist = m_orient.fwd.Distance(m_pos);
				//m_orient.fwd -= m_pos;
				//if(dist > m_arenasize.x / 2)
				//	m_orient.fwd *= -1;
				//m_orient.fwd.Normalize();
			}

			CFPoint fpos(m_pos);
			fpos.MulAdd(m_orient.fwd, fTime * m_screensize.x * speed);

#if 0
			// Avoid moving too close to other hunters.
			IGameObject* pObj = m_pObjects->GetFirst();
			while(pObj != nullptr)
			{
				int id = pObj->GetID();
				if(id != 1 && id != 6) // not the player or human
				{
					float dist1 = m_pos.distance(pObj->m_pos);
					float dist2 = fpos.Distance(pObj->m_pos);
					if(dist1 < m_personalSpace)
					{
						if(dist2 < dist1)
						{
							// Reverse course at reduced speed.
							speed *= 0.9f;
							m_orient.fwd *= -1;
							m_orient.fwd.y *= 2.0f;
							break;
						}
						else
						{
							speed *= 0.7f;
							m_orient.fwd.y *= 1.0f;
						}
					}
				}
				pObj = pObj->GetNext();
			}
#endif

			m_pos.MulAdd(m_orient.fwd, fTime * m_screensize.x * speed);
		}
			break;
	} // switch(state)

	m_inertia = m_pos - m_inertia;

	//m_fNow = GameTime();
}


void Defcon::CLander::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CLander::GetExplosionColorBase() const
{
	//return CGameColors::lightyellow;
 	//return CGameColors::yellow;
	return CGameColors::green;
}
