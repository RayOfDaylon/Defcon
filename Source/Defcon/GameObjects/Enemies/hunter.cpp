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



Defcon::CHunter::CHunter()
{
	m_parentType = m_type;
	m_type = ObjType::HUNTER;
	m_pointValue = HUNTER_VALUE;
	m_eState = lounging;
	m_pTarget = nullptr;
	float speed = FRANDRANGE(HUNTER_SPEEDMIN, HUNTER_SPEEDMAX);
	if(FRAND > 0.5f)
	{
		speed *= -1;
	}
	m_orient.fwd.Set(speed, 0);
	m_personalSpace = FRAND * 60 + 20;
	m_smallColor = C_ORANGE;
	//m_bboxrad.Set(15, 15);
	m_fAnimSpeed = FRAND * 0.6f + 0.4f;
	m_fBrightness = 1.0f;
	m_fTimeTargetWithinRange = 0.0f;
	m_amp = FRANDRANGE(0.33f, 0.9f);

	CreateSprite(m_type);
	const auto& Info = GameObjectResources.Get(m_type);
	m_bboxrad.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::CHunter::~CHunter()
{
}


void Defcon::CHunter::OnAboutToDie()
{
}


#ifdef _DEBUG
const char* Defcon::CHunter::GetClassname() const
{
	static char* psz = "Hunter";
	return psz;
};
#endif


void Defcon::CHunter::Notify(Defcon::Message msg, void* pObj)
{
	CEnemy::Notify(msg, pObj);
}


void Defcon::CHunter::Move(float fTime)
{
	CEnemy::Move(fTime);

	// todo: set current sprite cel accordingly.
	// The left-facing cels are from 0-5, the right-facing cels are from 6-11.
	//int32 CelBase = m_fAge / 

	const auto SecondsPerFrame = 1.0f / 15.0f; // assume 15 fps

	//Sprite->SetCurrentCel(m_orient.fwd.x < 0 ? 0 : 6 + ((FMath::RoundToInt(m_fAge / SecondsPerFrame)) % 6) /* numcels/2*/);

	//CurrentAge += DeltaTime;

	// todo: computing AnimDuration and having our own CurrentAge for anim would be smoother.
	/* const auto AnimDuration = Atlas.NumCels * SecondsPerFrame;

	while(CurrentAge > AnimDuration)
	{
		CurrentAge -= Atlas.NumCels / Atlas.FrameRate;
	}*/


	// Move towards target.

	m_inertia = m_pos;
	
	IGameObject* pTarget = m_pTarget;

	if(pTarget == nullptr)
		m_fTimeTargetWithinRange = 0.0f;
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

				m_targetOffset.Set(
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

				//m_orient.fwd.Set(SGN(this->m_targetOffset.y), 0);
				m_orient.fwd.y += (float)(m_amp * sin(m_fAge * m_freq));
				m_pos.MulAdd(m_orient.fwd, fTime * HUNTER_SPEEDMIN/2);
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
					m_pos.MulAdd(m_orient.fwd, fTime * AVG(HUNTER_SPEEDMIN, HUNTER_SPEEDMAX));
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
				if(ABS(vd) < m_bboxrad.y
					&& SGN(pTarget->m_orient.fwd.x) != 
					SGN(m_orient.fwd.x))
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
						speed = MAP(dist, 0.8f, 1.0f, HUNTER_SPEEDMAX, 0.0f);
					else
						speed = MAP(dist, 0.0f, 0.8f, HUNTER_SPEEDMIN, HUNTER_SPEEDMAX);

					m_orient.fwd.y += (float)(m_amp * sin(m_fAge * m_freq));
					m_pos.MulAdd(m_orient.fwd, fTime * speed);

					if(this->CanBeInjured() 
						&& pTarget->CanBeInjured()
						&& speed < 400 && FRAND <= 0.07f)
						gpArena->FireBullet(*this, m_pos, 1, 1);

				}
			}
		}
			break;
	} // switch(state)

	Sprite->FlipHorizontal = (m_orient.fwd.x < 0);

	// Constrain vertically.
	m_pos.y = CLAMP(m_pos.y, 0, gpArena->GetHeight());

	m_inertia = m_pos - m_inertia;
}


void Defcon::CHunter::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CHunter::GetExplosionColorBase() const
{
	return CGameColors::yellow;
}


float Defcon::CHunter::GetExplosionMass() const
{
	return 1.5f;
}
