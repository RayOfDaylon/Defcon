// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	pod.cpp
	Pod enemy type for Defcon game.
*/

#include "pod.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"
#include "DefconLogging.h"



Defcon::CPod::CPod()
	:
	m_yoff(0.0f),
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	m_parentType = m_type;
	m_type = ObjType::POD;
	m_pointValue = POD_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_MAGENTA;
	m_fAnimSpeed = FRAND * 0.35f + 0.65f;
	m_xFreq = FRAND * 0.5f + 1.0f;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CPod::~CPod()
{
}


#ifdef _DEBUG
const char* Defcon::CPod::GetClassname() const
{
	static char* psz = "Pod";
	return psz;
};
#endif


void Defcon::CPod::Move(float fTime)
{
	// Just float around drifting horizontally.

	CEnemy::Move(fTime);
	m_inertia = m_pos;

	//UE_LOG(LogGame, Log, TEXT("%S: Pod is at %d, %d"), __FUNCTION__, (int32)m_pos.x, (int32)m_pos.y);

	m_orient.fwd.y = 0.1f * (float)sin(m_freq * (m_yoff + m_fAge)); 

	// Cause radar blip to blink
	//float f = (float)fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed;
	float f = 0.5f * (sin(m_fAge * PI * 2) + 1.0f);
	//f *= 1;
	LerpColor(C_RED, C_BLUE, f, m_smallColor);


	m_pos.muladd(m_orient.fwd, fTime * 50.0f);

	//UE_LOG(LogGame, Log, TEXT("%S: Pod now at %d, %d"), __FUNCTION__, (int32)m_pos.x, (int32)m_pos.y);

	m_inertia = m_pos - m_inertia;
}


void Defcon::CPod::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CPod::OnAboutToDie()
{
	// Release swarmers.

	for(int32 i = 0; i < SWARMERS_PER_POD; i++)
	{
		gpArena->CreateEnemy(ObjType::SWARMER, m_pos, 0.0f, false, false);
	}
}


void Defcon::CPod::Explode(CGameObjectCollection& debris)
{
	// Pods explode normally (but deep purple or red)
	// but in a pod intersection, even more so because 
	// all the swarmers are biting it.

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	int n = (int)(FRAND * 30 + 30);
	float maxsize = FRAND * 5 + 3;

/*
	// Don't use huge particles ever; it 
	// just looks silly in the end.
	if(FRAND < .08)
		maxsize = 14;
*/

	// Define which color to make the debris.

	int	cby = CGameColors::purple;
	maxsize *= 1.5f;


	bool bDieOff = (FRAND >= 0.25f);
	int i;


	for(i = 0; i < n; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = cby;
		pFlak->m_fLargestSize = maxsize;
		pFlak->m_bFade = bDieOff;
		pFlak->m_bCold = true;

		pFlak->m_pos = m_pos;
		pFlak->m_orient = m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
		// Make the particle have a velocity vector
		// as if it were standing still.
		float speed = FRAND * 180 + 90;


		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		n = (int)(FRAND * 20 + 20);
		maxsize = FRAND * 4 + 8.0f;
		//maxsize = FMath::Min(maxsize, 9.0f);


		for(i = 0; i < n; i++)
		{
			CFlak* pFlak = new CFlak;
			pFlak->m_eColorbaseYoung = cby;
			pFlak->m_eColorbaseOld = cby;
			pFlak->m_fLargestSize = maxsize;
			pFlak->m_bFade = bDieOff;
			pFlak->m_bDrawSmall = false;
			pFlak->m_bCold = true;

			pFlak->m_pos = m_pos;
			pFlak->m_orient = m_orient;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.set((float)cos(t), (float)sin(t));

			pFlak->m_orient.fwd = m_inertia;

			pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
			float speed = FRAND * 45 + 22;

			pFlak->m_orient.fwd.muladd(dir, speed);

			debris.Add(pFlak);
		}
	}
}
