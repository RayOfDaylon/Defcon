// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	reformer.cpp
	Reformer enemy type for Defcon game.
*/


#include "reformer.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"



Defcon::CReformer::CReformer()
	:
	m_yoff(0.0f),
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	m_parentType = m_type;
	m_type = ObjType::REFORMER;
	m_pointValue = REFORMER_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = MakeColorFromComponents(191, 33, 33);
	m_bboxrad.set(10, 10); // todo: why so explicit?//(float)bmp.GetWidth()/2, (float)bmp.GetHeight()/2);
	m_fAnimSpeed = FRAND * 0.35f + 0.65f;
	m_xFreq = FRAND * 0.5f + 1.0f;
	m_bWaits = BRAND;
	m_numParts = IRAND(4) + 4;
	m_fSpinVel = SFRAND;
	m_fSpinAngle = FRAND;
	m_fSpinVelMax = FRAND * 4.0f + 1.0f;
}


Defcon::CReformer::~CReformer()
{
}


#ifdef _DEBUG
const char* Defcon::CReformer::GetClassname() const
{
	static char* psz = "Reformer";
	return psz;
};
#endif


void Defcon::CReformer::Move(float fTime)
{
	// Just float around drifting horizontally.

	CEnemy::Move(fTime);
	m_inertia = m_pos;

	m_orient.fwd.y = 0.1f * (float)sin(m_freq * (m_yoff + m_fAge)); 

	float diff = (float)gDefconGameInstance->GetScore() / 50000;

	if(m_bWaits)
		diff *= (float)(ABS(sin(m_fAge*PI)));
	diff = FMath::Min(diff, 1.5f);

	if(true/*m_bFiresBullets*/)
	{
		if(FRAND <= 0.05f * diff
			&& this->CanBeInjured()
			&& gpArena->GetPlayerShip().IsAlive()
			&& gpArena->IsPointVisible(m_pos))
			gpArena->FireBullet(*this, m_pos, 1, 1);
	}


	m_fSpinVel = (float)(sin(m_fAge * PI * m_fSpinVelMax));
	m_fSpinAngle += (m_fSpinVel * fTime);

	m_pos.muladd(m_orient.fwd, fTime * 50.0f);

	m_inertia = m_pos - m_inertia;
}


void Defcon::CReformer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	CEnemy::Draw(framebuf, mapper);


	m_partLocs[0] = m_pos;

	float f = (float)fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed;


	// Draw the parts in a circle around a central part.
	size_t n = m_numParts - 1;
	size_t i;
	for(i = 0; i < n; i++)
	{
		const float t = (float)(TWO_PI * i / n + (m_fSpinAngle * TWO_PI));
		CFPoint pt2((float)cos(t), (float)sin(t));
		float r = (float)(sin(f * PI) * 5 + 10);
		m_bboxrad.set(r, r);
		pt2 *= r;
		pt2 += m_pos;
		m_partLocs[i+1] = pt2;
	}

	for(i = 0; i < m_numParts; i++)
	{
		CFPoint pt;
		mapper.To(m_partLocs[i], pt);
		this->DrawPart(framebuf, pt);
	}
}


void Defcon::CReformer::DrawPart(FPaintArguments& framebuf, const CFPoint& where)
{
	auto& Info = GameObjectResources.Get(ObjType::REFORMERPART);

	CFPoint pt = where;
	const int w = Info.Size.X;

	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float f = (NumCels - 1) * PSIN(PI * fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed);

		const float Usize = 1.0f / NumCels;

		const auto S = Info.Size;
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

		const float F = Usize * ROUND(f);
		FBox2f UVRegion(FVector2f(F, 0.0f), FVector2f(F + Usize, 1.0f));
		Info.Atlas->Atlas.AtlasBrush.SetUVRegion(UVRegion);

		FSlateDrawElement::MakeBox(
			*framebuf.OutDrawElements,
			framebuf.LayerId,
			Geometry.ToPaintGeometry(),
			&Info.Atlas->Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			C_WHITE * framebuf.RenderOpacity);
	}
}


void Defcon::CReformer::OnAboutToDie()
{
	// Release parts.

	for(size_t i = 0; i < m_numParts; i++)
	{
		gpArena->CreateEnemy(ObjType::REFORMERPART, m_partLocs[i], 0.0f, false, false);
	}
}


void Defcon::CReformer::Explode(CGameObjectCollection& debris)
{
	// Pods explode normally (but deep purple or red)
	// but in a pod intersection, even more so because 
	// all the swarmers are biting it.

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();


#if 1
	const int cby = CGameColors::gray;


	for(size_t i = 0; i < 20; i++)
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
#endif
}

// --------------------------------------------------------


Defcon::CReformerPart::CReformerPart()
	:
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	m_parentType = m_type;
	m_type = ObjType::REFORMERPART;
	m_pointValue = REFORMERPART_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_RED;
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = 2.0f * FRANDRANGE(0.5f, 1.5f);
	m_bCanBeInjured = true;
	m_bIsCollisionInjurious = true;
	m_yoff = (float)(FRAND * PI);
	m_fTimeTargetWithinRange = 0.0f;
	m_fMergeTime = 0.0f;

	CreateSprite(m_type);
	const auto& Info = GameObjectResources.Get(m_type);
	m_bboxrad.set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::CReformerPart::~CReformerPart()
{
}


#ifdef _DEBUG
const char* Defcon::CReformerPart::GetClassname() const
{
	static char* psz = "Reformer_part";
	return psz;
};
#endif


void Defcon::CReformerPart::Move(float fTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Move(fTime);
	m_inertia = m_pos;

	// todo: if neighbouring parts are within visual range, 
	// then move towards them. If close enough and 
	// maximum number reached or other parts are too distant,
	// generate a reformer and delete the parts. 

	bool bMerging = (m_fAge >= 2.0f);


	IGameObject* pClosest = nullptr;
	CFPoint bestdir;
	float bestdist = 1.0e+10f;

	if(bMerging)
	{
		IGameObject* pObj = nullptr;
		for(;;)
		{
			IGameObject* pNeighbour = gpArena->FindEnemy(this->GetType(), pObj);
			if(pNeighbour == nullptr)
				break;

			pObj = pNeighbour;

			if(pNeighbour == this)
				continue;


			CFPoint dir;
			float dist = gpArena->Direction(m_pos, pNeighbour->m_pos, dir);
			if(dist < bestdist)
			{
				pClosest = pNeighbour;
				bestdist = dist;
				bestdir = dir;
			}
		}
	}


	if(pClosest == nullptr)
	{
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

			if(m_fTimeTargetWithinRange
				&& m_fAge > 1.0f 
				&& FRAND <= 0.007f
				&& SGN(m_orient.fwd.x) == SGN(dir.x))
			{
				gpArena->FireBullet(*this, m_pos, 1, 1);
				gpAudio->OutputSound(Defcon::snd_swarmer);
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
	}
	else
	{
		if(bestdist > 15.0f) 
			m_pos = m_inertia + (bestdir * (FRAND*50+150) * fTime);
		else
		{
			// If we have been at rest for longer than 
			// 2 seconds, then form a reformer.
			m_fMergeTime += fTime;
			if(m_fMergeTime >= 2.0f)
			{
				this->MarkAsDead();
				pClosest->MarkAsDead();
				m_pos.avg(pClosest->m_pos);
				gpArena->CreateEnemy(ObjType::REFORMER, m_pos, 0.0f, false, false);
			}
		}
	}

	m_inertia = m_pos - m_inertia;
}


void Defcon::CReformerPart::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CReformerPart::Explode(CGameObjectCollection& debris)
{
	const int cby = CGameColors::gray;

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	for(size_t i = 0; i < 20; i++)
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
