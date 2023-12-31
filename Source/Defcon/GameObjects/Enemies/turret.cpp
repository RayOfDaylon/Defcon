// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	turret.cpp
	Turret type for Defcon game.
*/


#include "turret.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CTurret::CTurret()
	:
	m_freq(2.0f)
{
	Type						= EObjType::TURRET;
	bGrounded					= true;
	PointValue				= SWARMER_VALUE; // todo: use TURRET_VALUE
	bCanBeInjured				= true;
	bIsCollisionInjurious		= true;
	RadarColor				= C_RED;
	AnimSpeed				= FRAND * 0.35f + 0.15f;
	m_fTimeTargetWithinRange	= 0.0f;

	Inertia.Set(0.0f, 0.0f);
	Orientation.Fwd.Set(0.0f, 1.0f);

	//CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::turret);
	//BboxRadius.Set(bmp.GetWidth() / 2.0f, bmp.GetHeight() / 2.0f);

	SetShieldStrength(10.0f); // can take ten laser hits

	//m_bMaterializes = false;
	//m_yoff = (float)(FRAND * PI);
	//m_xFreq = 2.0f * LERP(0.5f, 1.5f, FRAND);
}


Defcon::CTurret::~CTurret()
{
}


Defcon::EColor Defcon::CTurret::GetExplosionColorBase() const
{
	return EColor::LightYellow;
}


void Defcon::CTurret::Tick(float DeltaTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Tick(DeltaTime);

	//Inertia = Position;

	if(TargetPtr == nullptr)
	{
		m_fTimeTargetWithinRange = 0.0f;
	}
	else
	{
		const bool bVis = IsOurPositionVisible();

		// Update target-within-range information.
		if(m_fTimeTargetWithinRange > 0.0f)
		{
			// Target was in range; See if it left range.
			if(!bVis)
				m_fTimeTargetWithinRange = 0.0f;
			else
				m_fTimeTargetWithinRange += DeltaTime;
		}
		else
		{
			// Target was out of range; See if it entered range.
			if(bVis)
			{
				m_fTimeTargetWithinRange = DeltaTime;

				//m_targetOffset.Set(
				//	LERP(-100, 100, FRAND), 
				//	LERP(50, 90, FRAND) * SGN(Position.y - pTarget->Position.y));
				//Frequency = LERP(6, 12, FRAND);
				//m_amp = LERP(.33f, .9f, FRAND);
			}
		}

		CFPoint dir;
		float dist = GArena->ShortestDirection(Position, TargetPtr->Position, dir);

		/*if(m_fTimeTargetWithinRange > 0.25f)
		{
			if(dist > ScreenSize.x * .4f)
			{
				Orientation.Fwd = dir;
				Orientation.Fwd.y = 0;
				Orientation.Fwd.Normalize();
			}
		}*/
		// todo: this is framerate dependant.
		if(m_fTimeTargetWithinRange && Age > 1.0f && FRAND <= 0.07f
			/*&& SGN(Orientation.Fwd.x) == SGN(dir.x)*/)
		{
			GArena->FireBullet(*this, Position, 1, 1);
			//GMessageMediator.PlaySound(Defcon::swarmer);
		}
	}


#if 0
	m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff+Age)) * 0.5f * ScreenSize.y;
	m_halfwayAltitude = (float)(sin((m_yoff+Age)*0.6f) * 50 + (0.5f * ScreenSize.y));

	CFPoint pos;
	if(Age < 0.7f)
		pos.x = Position.x + .2f * Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);
	else
		pos.x = Position.x + Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);
	pos.y = 
		sinf(Frequency * (m_yoff + Age)) 
		* m_amp + m_halfwayAltitude;

	Position = pos;
	if(Age < 0.7f)
		Position.y = LERP(m_posOrg.y, pos.y, Age / 0.7f);

	Inertia = Position - Inertia;
#endif
}


void Defcon::CTurret::Explode(CGameObjectCollection& Debris)
{
	Super::Explode(Debris);
#if 0
	const int32 cby = EColor::Red;

	bMortal = true;
	Lifespan = 0.0f;
	OnAboutToDie();

	for(int32 i = 0; i < 100; i++)
	{
		CFlak* pFlak = new CFlak;
		Flak->SetCreatorType(GetType());

		pFlak->ColorbaseYoung = EColor::LightYellow;
		pFlak->ColorbaseOld	= cby;
		pFlak->bCold			= true;
		pFlak->LargestSize	= FRAND * 15 + 2;
		pFlak->bFade			= true;//bDieOff;
		pFlak->Lifespan		= 1.0f;

		pFlak->Position			= Position;
		pFlak->Orientation			= Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set(cosf(t), (float)abs(sin(t)));

#if 1
		pFlak->Orientation.Fwd.Set(0.0f, 0.0f);

		// Debris has at least the object's momentum.
		//pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		//pFlak->Orientation.Fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 600 + 100;

		pFlak->Orientation.Fwd.MulAdd(dir, speed);
#endif
		debris.Add(pFlak);
	}
#endif
}
