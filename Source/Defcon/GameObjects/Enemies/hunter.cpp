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
	ParentType = Type;
	Type = EObjType::HUNTER;
	PointValue = HUNTER_VALUE;
	m_eState = lounging;
	TargetPtr = nullptr;
	float speed = FRANDRANGE(HUNTER_SPEEDMIN, HUNTER_SPEEDMAX);
	if(FRAND > 0.5f)
	{
		speed *= -1;
	}
	Orientation.Fwd.Set(speed, 0);
	m_personalSpace = FRAND * 60 + 20;
	RadarColor = C_ORANGE;
	//BboxRadius.Set(15, 15);
	AnimSpeed = FRAND * 0.6f + 0.4f;
	Brightness = 1.0f;
	m_fTimeTargetWithinRange = 0.0f;
	m_amp = FRANDRANGE(0.33f, 0.9f);

	CreateSprite(Type);
	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
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


void Defcon::CHunter::Move(float fTime)
{
	CEnemy::Move(fTime);

	// todo: set current sprite cel accordingly.
	// The left-facing cels are from 0-5, the right-facing cels are from 6-11.
	//int32 CelBase = Age / 

	const auto SecondsPerFrame = 1.0f / 15.0f; // assume 15 fps

	//Sprite->SetCurrentCel(Orientation.Fwd.x < 0 ? 0 : 6 + ((FMath::RoundToInt(Age / SecondsPerFrame)) % 6) /* numcels/2*/);

	//CurrentAge += DeltaTime;

	// todo: computing AnimDuration and having our own CurrentAge for anim would be smoother.
	/* const auto AnimDuration = Atlas.NumCels * SecondsPerFrame;

	while(CurrentAge > AnimDuration)
	{
		CurrentAge -= Atlas.NumCels / Atlas.FrameRate;
	}*/


	// Move towards target.

	Inertia = Position;
	
	IGameObject* pTarget = TargetPtr;

	if(pTarget == nullptr)
		m_fTimeTargetWithinRange = 0.0f;
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
				gpArena->Direction(Position, pTarget->Position, Orientation.Fwd);

				//Orientation.Fwd.Set(SGN(this->m_targetOffset.y), 0);
				Orientation.Fwd.y += (float)(m_amp * sin(Age * m_freq));
				Position.MulAdd(Orientation.Fwd, fTime * HUNTER_SPEEDMIN/2);
			}
			break;


		case evading:
		{
			if(pTarget != nullptr)
			{
				float vd = Position.y - pTarget->Position.y;
				if(ABS(vd) > BboxRadius.y || 
					SGN(pTarget->Orientation.Fwd.x) == 
					SGN(Orientation.Fwd.x))
					m_eState = fighting;
				else
				{
					Orientation.Fwd.y = SGN(vd)*0.5f;
					//if(Orientation.Fwd.y == 0)
					//	Orientation.Fwd.y = SFRAND;
					CFPoint pt;
					gpArena->Direction(Position, pTarget->Position, pt);
					Orientation.Fwd.x = (FRAND * 0.25f + 0.33f) * SGN(pt.x);
					Position.MulAdd(Orientation.Fwd, fTime * AVG(HUNTER_SPEEDMIN, HUNTER_SPEEDMAX));
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

				float dist = gpArena->Direction(Position, pTarget->Position, Orientation.Fwd);
				float vd = Position.y - pTarget->Position.y;
				if(ABS(vd) < BboxRadius.y
					&& SGN(pTarget->Orientation.Fwd.x) != 
					SGN(Orientation.Fwd.x))
				{
					// We can be hit by player. 
					// Take evasive action.
					m_eState = evading;
				}
				else
				{
					CFPoint pt = pTarget->Position + m_targetOffset;
					gpArena->Direction(Position, pt, Orientation.Fwd);

					float speed;

					dist /= gpArena->GetDisplayWidth();
					if(dist >= 0.8f)
						speed = MAP(dist, 0.8f, 1.0f, HUNTER_SPEEDMAX, 0.0f);
					else
						speed = MAP(dist, 0.0f, 0.8f, HUNTER_SPEEDMIN, HUNTER_SPEEDMAX);

					Orientation.Fwd.y += (float)(m_amp * sin(Age * m_freq));
					Position.MulAdd(Orientation.Fwd, fTime * speed);

					if(this->CanBeInjured() 
						&& pTarget->CanBeInjured()
						&& speed < 400 && FRAND <= 0.07f)
						gpArena->FireBullet(*this, Position, 1, 1);

				}
			}
		}
			break;
	} // switch(state)

	Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);

	// Constrain vertically.
	Position.y = CLAMP(Position.y, 0, gpArena->GetHeight());

	Inertia = Position - Inertia;
}


void Defcon::CHunter::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


Defcon::EColor Defcon::CHunter::GetExplosionColorBase() const
{
	return EColor::Yellow;
}


float Defcon::CHunter::GetExplosionMass() const
{
	return 1.5f;
}
