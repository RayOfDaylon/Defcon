// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	turret.cpp
	Turret type for Defcon game.
	Copyright 2021 Daylon Graphics Ltd.
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
	m_type						= ObjType::TURRET;
	m_grounded					= true;
	m_pointValue				= SWARMER_VALUE; // todo: use TURRET_VALUE
	m_bCanBeInjured				= true;
	m_bIsCollisionInjurious		= true;
	m_smallColor				= C_RED;
	m_fAnimSpeed				= FRAND * 0.35f + 0.15f;
	m_fTimeTargetWithinRange	= 0.0f;

	m_inertia.Set(0.0f, 0.0f);
	m_orient.fwd.Set(0.0f, 1.0f);

	//CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::turret);
	//m_bboxrad.Set(bmp.GetWidth() / 2.0f, bmp.GetHeight() / 2.0f);

	SetShieldStrength(10.0f); // can take ten laser hits

	//m_bMaterializes = false;
	//m_yoff = (float)(FRAND * PI);
	//m_xFreq = 2.0f * LERP(0.5f, 1.5f, FRAND);
}


Defcon::CTurret::~CTurret()
{
}


#ifdef _DEBUG
const char* Defcon::CTurret::GetClassname() const
{
	static char* psz = "Turret";
	return psz;
};
#endif


int Defcon::CTurret::GetExplosionColorBase() const
{
	return CGameColors::lightyellow;
}


void Defcon::CTurret::Move(float fTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Move(fTime);


	//m_inertia = m_pos;


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

				//m_targetOffset.Set(
				//	LERP(-100, 100, FRAND), 
				//	LERP(50, 90, FRAND) * SGN(m_pos.y - pTarget->m_pos.y));
				//m_freq = LERP(6, 12, FRAND);
				//m_amp = LERP(.33f, .9f, FRAND);
			}
		}

		CFPoint dir;
		float dist = gpArena->Direction(m_pos, pTarget->m_pos, dir);

		/*if(m_fTimeTargetWithinRange > 0.25f)
		{
			if(dist > m_screensize.x * .4f)
			{
				m_orient.fwd = dir;
				m_orient.fwd.y = 0;
				m_orient.fwd.Normalize();
			}
		}*/
		// todo: this is framerate dependant.
		if(m_fTimeTargetWithinRange && m_fAge > 1.0f && FRAND <= 0.07f
			/*&& SGN(m_orient.fwd.x) == SGN(dir.x)*/)
		{
			gpArena->FireBullet(*this, m_pos, 1, 1);
			//gpAudio->OutputSound(Defcon::snd_swarmer);
		}
	}


#if 0
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
#endif
}


void Defcon::CTurret::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
#if 0
	CEnemy::Draw(framebuf, mapper);
	if(this->IsMaterializing())
		return;

	CFPoint pt;
	mapper.To(m_pos, pt);


	float f = (float)fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed;
	//f = (float)cos(f * PI) + 1.0f;
	f *= 0;
	
	CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::turret + ROUND(f));
	int w = bmp.GetWidth();
	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		pt.sub(CFPoint(w / 2.0f, bmp.GetHeight() / 2.0f));
		bmp.BlitAlphaBrighten(
			framebuf, ROUND(pt.x), ROUND(pt.y), 
			w, bmp.GetHeight(), 
			0, 0, /*m_fBrightness * */ (FRAND * 0.1f + 0.9f));
	}
#endif
}


void Defcon::CTurret::Explode(CGameObjectCollection& debris)
{
	Super::Explode(debris);
#if 0
	const int cby = CGameColors::red;

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 100; i++)
	{
		CFlak* pFlak = new CFlak;

		pFlak->m_eColorbaseYoung = CGameColors::lightyellow;
		pFlak->m_eColorbaseOld	= cby;
		pFlak->m_bCold			= true;
		pFlak->m_fLargestSize	= FRAND * 15 + 2;
		pFlak->m_bFade			= true;//bDieOff;
		pFlak->m_fLifespan		= 1.0f;

		pFlak->m_pos			= m_pos;
		pFlak->m_orient			= m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)abs(sin(t)));

#if 1
		pFlak->m_orient.fwd.Set(0.0f, 0.0f);

		// Debris has at least the object's momentum.
		//pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		//pFlak->m_orient.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 600 + 100;

		pFlak->m_orient.fwd.MulAdd(dir, speed);
#endif
		debris.Add(pFlak);
	}
#endif
}
