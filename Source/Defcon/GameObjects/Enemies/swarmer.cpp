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
	m_parentType = m_type;
	m_type = ObjType::SWARMER;
	m_pointValue = SWARMER_VALUE;

	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_ORANGE;
	
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = 2.0f * FRANDRANGE(0.5f, 1.5f);
	m_bCanBeInjured = true;
	m_bIsCollisionInjurious = true;
	m_yoff = (float)(FRAND * PI);
	m_fTimeTargetWithinRange = 0.0f;

	CreateSprite(m_type);

	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
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

				//m_targetOffset.set(
				//	LERP(-100, 100, FRAND), 
				//	LERP(50, 90, FRAND) * SGN(m_pos.y - pTarget->m_pos.y));
				//m_freq = LERP(6, 12, FRAND);
				//m_amp = LERP(.33f, .9f, FRAND);
			}
		}

		CFPoint dir;
		float dist = gpArena->Direction(m_pos, pTarget->m_pos, dir);

		if(m_fTimeTargetWithinRange > 0.75f)
		{
			if(dist > m_screensize.x * .4f)
			{
				m_orient.fwd = dir;
				m_orient.fwd.y = 0;
				m_orient.fwd.normalize();
			}
		}


		if(m_fTimeTargetWithinRange	&& m_fAge > 1.0f)
		{
			CountdownToMakeSound -= fTime;

			if(CountdownToMakeSound <= 0.0f)
			{
				gpAudio->OutputSound(Defcon::snd_swarmer);
				CountdownToMakeSound = Daylon::FRandRange(SWARMER_SOUND_COUNTDOWN_MIN, SWARMER_SOUND_COUNTDOWN_MAX);
			}

			if(FRAND <= 0.007f && SGN(m_orient.fwd.x) == SGN(dir.x))
			{
				gpArena->FireBullet(*this, m_pos, 1, 1);
			}
		}
	}


	m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff+m_fAge)) * 0.5f * m_screensize.y;
	m_halfwayAltitude = (float)(sin((m_yoff+m_fAge)*0.6f) * 50 + (0.5f * m_screensize.y));

	CFPoint pos;
	if(m_fAge < 0.7f)
		pos.x = m_pos.x + .2f * m_orient.fwd.x * m_xFreq * fTime * m_screensize.x * (FRAND * .05f + 0.25f);
	else
		pos.x = m_pos.x + m_orient.fwd.x * m_xFreq * fTime * m_screensize.x * (FRAND * .05f + 0.25f);

	pos.y = 
		(float)sin(m_freq * (m_yoff + m_fAge)) 
		* m_amp + m_halfwayAltitude;

	m_pos = pos;
	if(m_fAge < 0.7f)
		m_pos.y = LERP(m_posOrg.y, pos.y, m_fAge / 0.7f);

	m_inertia = m_pos - m_inertia;
}


void Defcon::CSwarmer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CSwarmer::Explode(CGameObjectCollection& debris)
{
	const int cby = CGameColors::red;

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = cby;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 4;
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
