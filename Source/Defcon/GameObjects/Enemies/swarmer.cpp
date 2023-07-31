// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	swarmer.cpp
	Swarmer type for Defcon game.
*/


#include "swarmer.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"


constexpr float SWARMER_SOUND_COUNTDOWN_MIN = 0.75f;
constexpr float SWARMER_SOUND_COUNTDOWN_MAX = 1.50f;


Defcon::CSwarmer::CSwarmer()
	:
	m_freq(2.0f),
	m_xFreq(1.0f),
	CountdownToMakeSound(Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX))
{
	ParentType = Type;
	Type = EObjType::SWARMER;
	PointValue = SWARMER_VALUE;

	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_ORANGE;
	
	AnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = 2.0f * FRANDRANGE(0.5f, 1.5f);
	bCanBeInjured = true;
	bIsCollisionInjurious = true;
	m_yoff = (float)(FRAND * PI);
	m_fTimeTargetWithinRange = 0.0f;

	CreateSprite(Type);

	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CSwarmer::~CSwarmer()
{
}


#ifdef _DEBUG
const char* Defcon::CSwarmer::GetClassname() const
{
	static char* psz = "Swarmer";
	return psz;
};
#endif


void Defcon::CSwarmer::Move(float fTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Move(fTime);
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

				//m_targetOffset.Set(
				//	LERP(-100, 100, FRAND), 
				//	LERP(50, 90, FRAND) * SGN(Position.y - pTarget->Position.y));
				//m_freq = LERP(6, 12, FRAND);
				//m_amp = LERP(.33f, .9f, FRAND);
			}
		}

		CFPoint dir;
		float dist = gpArena->Direction(Position, pTarget->Position, dir);

		if(m_fTimeTargetWithinRange > 0.75f)
		{
			if(dist > ScreenSize.x * .4f)
			{
				Orientation.Fwd = dir;
				Orientation.Fwd.y = 0;
				Orientation.Fwd.Normalize();
			}
		}


		if(m_fTimeTargetWithinRange	&& Age > 1.0f)
		{
			CountdownToMakeSound -= fTime;

			if(CountdownToMakeSound <= 0.0f)
			{
				gpAudio->OutputSound(Defcon::swarmer);
				CountdownToMakeSound = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);
			}

			if(FRAND <= 0.007f && SGN(Orientation.Fwd.x) == SGN(dir.x))
			{
				gpArena->FireBullet(*this, Position, 1, 1);
			}
		}
	}


	m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff+Age)) * 0.5f * ScreenSize.y;
	m_halfwayAltitude = (float)(sin((m_yoff+Age)*0.6f) * 50 + (0.5f * ScreenSize.y));

	CFPoint pos;
	if(Age < 0.7f)
		pos.x = Position.x + .2f * Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);
	else
		pos.x = Position.x + Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);

	pos.y = 
		(float)sin(m_freq * (m_yoff + Age)) 
		* m_amp + m_halfwayAltitude;

	Position = pos;
	if(Age < 0.7f)
		Position.y = LERP(m_posOrg.y, pos.y, Age / 0.7f);

	Inertia = Position - Inertia;
}


void Defcon::CSwarmer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CSwarmer::Explode(CGameObjectCollection& debris)
{
	const auto cby = EColor::red;

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = cby;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->Position = Position;
		pFlak->Orientation = Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.Fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->Orientation.Fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}
}
