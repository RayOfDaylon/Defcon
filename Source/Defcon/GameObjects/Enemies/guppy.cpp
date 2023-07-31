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
	m_eState = lounging;
	m_pTarget = nullptr;
	float speed = FRANDRANGE(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);
	if(FRAND > 0.5f) 
		speed *= -1;
	Orientation.fwd.Set(speed, 0);
	m_personalSpace = FRAND * 60 + 20;
	RadarColor = C_ORANGE;

	AnimSpeed = FRAND * 0.6f + 0.6f;
	m_fBrightness = 1.0f;
	m_fTimeTargetWithinRange = 0.0f;
	m_amp = FRANDRANGE(0.25f, 0.4f);
	m_firingCountdown = 1.0f;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2 - 1);
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


void Defcon::CGuppy::Move(float fTime)
{
	// Move towards target.

	CEnemy::Move(fTime);

	Inertia = Position;
	
	IGameObject* pTarget = m_pTarget;

	if(pTarget == nullptr)
	{
		m_fTimeTargetWithinRange = 0.0f;
	}
	else
	{
		const bool bVis = gpArena->IsPointVisible(Position);

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

				m_targetOffset.Set(
					FRANDRANGE(-100, 100), 
					FRANDRANGE(50, 90) * SGN(Position.y - pTarget->Position.y));
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
				gpArena->Direction(Position, pTarget->Position, Orientation.fwd);

				//Orientation.fwd.Set(SGN(this->m_targetOffset.y), 0);
				Orientation.fwd.y += (float)(m_amp * sin(Age * m_freq));
				Position.MulAdd(Orientation.fwd, fTime * GUPPY_SPEEDMIN/2);
			}
			break;


		case evading:
		{
			if(pTarget != nullptr)
			{
				float vd = Position.y - pTarget->Position.y;
				if(ABS(vd) > BboxRadius.y || 
					SGN(pTarget->Orientation.fwd.x) == 
					SGN(Orientation.fwd.x))
					m_eState = fighting;
				else
				{
					Orientation.fwd.y = SGN(vd)*0.5f;
					//if(Orientation.fwd.y == 0)
					//	Orientation.fwd.y = SFRAND;
					CFPoint pt;
					gpArena->Direction(Position, pTarget->Position, pt);
					Orientation.fwd.x = (FRAND * 0.25f + 0.33f) * SGN(pt.x);
					Position.MulAdd(Orientation.fwd, fTime * AVG(GUPPY_SPEEDMIN, GUPPY_SPEEDMAX));
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

				float dist = gpArena->Direction(Position, pTarget->Position, Orientation.fwd);
				float vd = Position.y - pTarget->Position.y;
				if(ABS(vd) < BboxRadius.y && SGN(pTarget->Orientation.fwd.x) != SGN(Orientation.fwd.x))
				{
					// We can be hit by player. 
					// Take evasive action.
					m_eState = evading;
				}
				else
				{
					CFPoint pt = pTarget->Position + m_targetOffset;
					gpArena->Direction(Position, pt, Orientation.fwd);

					float speed;

					dist /= gpArena->GetDisplayWidth();
					if(dist >= 0.8f)
						speed = MAP(dist, 0.8f, 1.0f, GUPPY_SPEEDMAX, 0.0f);
					else
						speed = MAP(dist, 0.0f, 0.8f, GUPPY_SPEEDMIN, GUPPY_SPEEDMAX);

					Orientation.fwd.y += (float)(m_amp * sin(Age * m_freq));
					Position.MulAdd(Orientation.fwd, fTime * speed);

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
	Position.y = CLAMP(Position.y, 0, gpArena->GetHeight());

	Inertia = Position - Inertia;
}


void Defcon::CGuppy::ConsiderFiringBullet()
{
	//if(!(this->CanBeInjured() && gpArena->GetPlayerShip().IsAlive() && gpArena->IsPointVisible(Position)))
	//{
	//	return;
	//}

	if(m_firingCountdown <= 0.0f)
	{
		IBullet* pb = gpArena->FireBullet(*this, Position, (FRAND <= 0.85f) ? 2 : 3, 1);

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

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = BRAND ? CGameColors::magenta : CGameColors::red;
		pFlak->m_eColorbaseOld = BRAND ? CGameColors::magenta : CGameColors::orange;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 6;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->Position = Position;
		pFlak->Orientation = Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->Orientation.fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->Orientation.fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}
}
