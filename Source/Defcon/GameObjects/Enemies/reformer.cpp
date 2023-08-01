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
	ParentType = Type;
	Type = EObjType::REFORMER;
	PointValue = REFORMER_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = MakeColorFromComponents(191, 33, 33);
	BboxRadius.Set(10, 10); // todo: why so explicit?//(float)bmp.GetWidth()/2, (float)bmp.GetHeight()/2);
	AnimSpeed = FRAND * 0.35f + 0.65f;
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
	Inertia = Position;

	Orientation.Fwd.y = 0.1f * (float)sin(m_freq * (m_yoff + Age)); 

	float diff = (float)gDefconGameInstance->GetScore() / 50000;

	if(m_bWaits)
		diff *= (float)(ABS(sin(Age*PI)));
	diff = FMath::Min(diff, 1.5f);

	if(true/*m_bFiresBullets*/)
	{
		if(FRAND <= 0.05f * diff
			&& this->CanBeInjured()
			&& gpArena->GetPlayerShip().IsAlive()
			&& gpArena->IsPointVisible(Position))
			gpArena->FireBullet(*this, Position, 1, 1);
	}


	m_fSpinVel = (float)(sin(Age * PI * m_fSpinVelMax));
	m_fSpinAngle += (m_fSpinVel * fTime);

	Position.MulAdd(Orientation.Fwd, fTime * 50.0f);

	Inertia = Position - Inertia;
}


void Defcon::CReformer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	m_partLocs[0] = Position;

	float f = (float)fmod(Age, AnimSpeed) / AnimSpeed;


	// Draw the parts in a circle around a central part.
	int32 n = m_numParts - 1;
	int32 i;
	for(i = 0; i < n; i++)
	{
		const float t = (float)(TWO_PI * i / n + (m_fSpinAngle * TWO_PI));
		CFPoint pt2((float)cos(t), (float)sin(t));
		float r = (float)(sin(f * PI) * 5 + 10);
		BboxRadius.Set(r, r);
		pt2 *= r;
		pt2 += Position;
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
	auto& Info = GameObjectResources.Get(EObjType::REFORMERPART);

	CFPoint pt = where;
	const int w = Info.Size.X;

	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float f = (NumCels - 1) * PSIN(PI * fmod(Age, AnimSpeed) / AnimSpeed);

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

	for(int32 i = 0; i < m_numParts; i++)
	{
		gpArena->CreateEnemy(EObjType::REFORMERPART, m_partLocs[i], 0.0f, EObjectCreationFlags::EnemyPart);
	}
}


void Defcon::CReformer::Explode(CGameObjectCollection& debris)
{
	// Pods explode normally (but deep purple or red)
	// but in a pod intersection, even more so because 
	// all the swarmers are biting it.

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();


#if 1
	const auto cby = EColor::Gray;


	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->ColorbaseYoung = cby;
		pFlak->ColorbaseOld = cby;
		pFlak->bCold = true;
		pFlak->LargestSize = 4;
		pFlak->bFade = true;//bDieOff;

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
#endif
}

// --------------------------------------------------------


Defcon::CReformerPart::CReformerPart()
	:
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	ParentType = Type;
	Type = EObjType::REFORMERPART;
	PointValue = REFORMERPART_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_RED;
	AnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = 2.0f * FRANDRANGE(0.5f, 1.5f);
	bCanBeInjured = true;
	bIsCollisionInjurious = true;
	m_yoff = (float)(FRAND * PI);
	m_fTimeTargetWithinRange = 0.0f;
	m_fMergeTime = 0.0f;

	CreateSprite(Type);
	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
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
	Inertia = Position;

	// todo: if neighbouring parts are within visual range, 
	// then move towards them. If close enough and 
	// maximum number reached or other parts are too distant,
	// generate a reformer and delete the parts. 

	bool bMerging = (Age >= 2.0f);


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
			float dist = gpArena->Direction(Position, pNeighbour->Position, dir);
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

			if(m_fTimeTargetWithinRange
				&& Age > 1.0f 
				&& FRAND <= 0.007f
				&& SGN(Orientation.Fwd.x) == SGN(dir.x))
			{
				gpArena->FireBullet(*this, Position, 1, 1);
				gpAudio->OutputSound(EAudioTrack::Swarmer);
			}
		}

		m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff+Age)) * 0.5f * ScreenSize.y;
		m_halfwayAltitude = (float)(sin((m_yoff+Age)*0.6f) * 50 + (0.5f * ScreenSize.y));

		CFPoint pos;

		if(Age < 0.7f)
		{
			pos.x = Position.x + .2f * Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);
		}
		else
		{
			pos.x = Position.x + Orientation.Fwd.x * m_xFreq * fTime * ScreenSize.x * (FRAND * .05f + 0.25f);
		}

		pos.y = (float)sin(m_freq * (m_yoff + Age)) * m_amp + m_halfwayAltitude;

		Position = pos;
		if(Age < 0.7f)
		{
			Position.y = LERP(m_posOrg.y, pos.y, Age / 0.7f);
		}
	}
	else
	{
		if(bestdist > 15.0f) 
		{
			Position = Inertia + (bestdir * (FRAND*50+150) * fTime);
		}
		else
		{
			// If we have been at rest for longer than 
			// 2 seconds, then form a reformer.
			m_fMergeTime += fTime;

			if(m_fMergeTime >= 2.0f)
			{
				this->MarkAsDead();
				pClosest->MarkAsDead();
				Position.Avg(pClosest->Position);
				gpArena->CreateEnemy(EObjType::REFORMER, Position, 0.0f, EObjectCreationFlags::EnemyPart);
			}
		}
	}

	Inertia = Position - Inertia;
}


void Defcon::CReformerPart::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CReformerPart::Explode(CGameObjectCollection& debris)
{
	const auto cby = EColor::Gray;

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->ColorbaseYoung = cby;
		pFlak->ColorbaseOld = cby;
		pFlak->bCold = true;
		pFlak->LargestSize = 4;
		pFlak->bFade = true;//bDieOff;

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
