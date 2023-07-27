/*
	guppy.cpp
	Yllabian Guppy type for Defcon game.
	Copyright 2004 Daylon Graphics Ltd.
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

// -------------------------------------------------


Defcon::CGuppy::CGuppy()
{
	m_parentType = m_type;
	m_type = ObjType::GUPPY;
	m_pointValue = GUPPY_VALUE;
	m_eState = lounging;
	m_pTarget = nullptr;
	float speed = FRANDRANGE(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);
	if(FRAND > 0.5f) 
		speed *= -1;
	m_orient.fwd.set(speed, 0);
	m_personalSpace = FRAND * 60 + 20;
	m_smallColor = C_ORANGE;

	m_fAnimSpeed = FRAND * 0.6f + 0.6f;
	m_fBrightness = 1.0f;
	m_fTimeTargetWithinRange = 0.0f;
	m_amp = FRANDRANGE(0.25f, 0.4f);
	m_firingCountdown = 1.0f;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2 - 1);
}



void Defcon::CGuppy::OnAboutToDie()
{
}

#ifdef _DEBUG
const char* Defcon::CGuppy::GetClassname() const
{
	static char* psz = "Guppy";
	return psz;
};
#endif

void Defcon::CGuppy::Notify(Defcon::Message msg, void* pObj)
{
	CEnemy::Notify(msg, pObj);
}



void Defcon::CGuppy::Move(float fTime)
{
	// Move towards target.

	CEnemy::Move(fTime);

	m_inertia = m_pos;
	
	IGameObject* pTarget = m_pTarget;

	if(pTarget == nullptr)
	{
		m_fTimeTargetWithinRange = 0.0f;
	}
	else
	{
		const bool bVis = gpArena->IsPointVisible(m_pos);

		// Update target-within-range information.
		if(m_fTimeTargetWithinRange > 0.0f)
		{
			// Target was in range; See if it left range.
			if(!bVis)
				m_fTimeTargetWithinRange = 0.0f;
			else
				m_fTimeTargetWithinRange += fTime;
		}
		else
		{
			// Target was out of range; See if it entered range.
			if(bVis)
			{
				m_fTimeTargetWithinRange = fTime;

				m_targetOffset.set(
					FRANDRANGE(-100, 100), 
					FRANDRANGE(50, 90) * SGN(m_pos.y - pTarget->m_pos.y));
				m_freq = FRANDRANGE(6, 12);
				m_amp = FRANDRANGE(0.33f, 0.9f);
			}
		}
	}


	switch(m_eState)
	{
		case lounging:
			// Just float towards target unless the target
			// has become in range.
			if(m_fTimeTargetWithinRange >= 0.33f)
				m_eState = fighting;
			else if(pTarget != nullptr)
			{
				CFPoint pt;
				gpArena->Direction(m_pos, pTarget->m_pos, m_orient.fwd);

				//m_orient.fwd.set(SGN(this->m_targetOffset.y), 0);
				m_orient.fwd.y += (float)(m_amp * sin(m_fAge * m_freq));
				m_pos.muladd(m_orient.fwd, fTime * GUPPY_SPEEDMIN/2);
			}
			break;


		case evading:
		{
			if(pTarget != nullptr)
			{
				float vd = m_pos.y - pTarget->m_pos.y;
				if(ABS(vd) > m_bboxrad.y || 
					SGN(pTarget->m_orient.fwd.x) == 
					SGN(m_orient.fwd.x))
					m_eState = fighting;
				else
				{
					m_orient.fwd.y = SGN(vd)*0.5f;
					//if(m_orient.fwd.y == 0)
					//	m_orient.fwd.y = SFRAND;
					CFPoint pt;
					gpArena->Direction(m_pos, pTarget->m_pos, pt);
					m_orient.fwd.x = (FRAND * 0.25f + 0.33f) * SGN(pt.x);
					m_pos.muladd(m_orient.fwd, fTime * AVG(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX));
				}
			}
			
		}	
			break;


		case fighting:
		{
			if(m_fTimeTargetWithinRange == 0.0f)
				m_eState = lounging;
			else
			{
				check(pTarget != nullptr);

				float dist = gpArena->Direction(m_pos, pTarget->m_pos, m_orient.fwd);
				float vd = m_pos.y - pTarget->m_pos.y;
				if(ABS(vd) < m_bboxrad.y && SGN(pTarget->m_orient.fwd.x) != SGN(m_orient.fwd.x))
				{
					// We can be hit by player. 
					// Take evasive action.
					m_eState = evading;
				}
				else
				{
					CFPoint pt = pTarget->m_pos + m_targetOffset;
					gpArena->Direction(m_pos, pt, m_orient.fwd);

					float speed;

					dist /= gpArena->GetDisplayWidth();
					if(dist >= 0.8f)
						speed = MAP(dist, 0.8f, 1.0f, GUPPY_SPEEDMAX, 0.0f);
					else
						speed = MAP(dist, 0.0f, 0.8f, GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);

					m_orient.fwd.y += (float)(m_amp * sin(m_fAge * m_freq));
					m_pos.muladd(m_orient.fwd, fTime * speed);

					if(this->CanBeInjured() && pTarget->CanBeInjured() && speed < 400)
					{
						m_firingCountdown -= fTime;

						ConsiderFiringBullet();
					}

				}
			}
		}
			break;
	} // switch(state)

	// Constrain vertically.
	m_pos.y = CLAMP(m_pos.y, 0, gpArena->GetHeight());

	m_inertia = m_pos - m_inertia;
}


void Defcon::CGuppy::ConsiderFiringBullet()
{
	//if(!(this->CanBeInjured() && gpArena->GetPlayerShip().IsAlive() && gpArena->IsPointVisible(m_pos)))
	//{
	//	return;
	//}

	if(m_firingCountdown <= 0.0f)
	{
		IBullet* pb = gpArena->FireBullet(*this, m_pos, (FRAND <= 0.85f) ? 2 : 3, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)gDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		m_firingCountdown = LERP(1.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


void Defcon::CGuppy::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CGuppy::GetExplosionColorBase() const
{
	return CGameColors::magenta;
}


float Defcon::CGuppy::GetExplosionMass() const
{
	return 0.3f;
}


void Defcon::CGuppy::Explode(CGameObjectCollection& debris)
{
	const int cby = CGameColors::magenta;

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = BRAND ? CGameColors::magenta : CGameColors::red;
		pFlak->m_eColorbaseOld = BRAND ? CGameColors::magenta : CGameColors::orange;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 6;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->m_pos = m_pos;
		pFlak->m_orient = m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}
}


