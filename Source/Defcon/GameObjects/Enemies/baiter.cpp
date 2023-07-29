// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	baiter.cpp
	Baiter type for Defcon game.

	The baiter is an enemy ship that normally 
	appears late in a wave, flies very fast 
	(usually faster than the player) and tries 
	to shoot the player.
*/


#include "baiter.h"

#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/obj_types.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconLogging.h"



Defcon::CBaiter::CBaiter()
{
	m_parentType = m_type;
	m_type = ObjType::BAITER;
	m_pointValue = BAITER_VALUE;
	m_smallColor = MakeColorFromComponents(192, 192, 0);
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;

	m_fDrag = PLAYER_DRAG * 0.15f;
	m_maxThrust = PLAYER_MAXTHRUST * 9.0f;
	m_fMass = PLAYER_MASS * 20;
	m_bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
}


Defcon::CBaiter::~CBaiter()
{
}

#ifdef _DEBUG
const char* Defcon::CBaiter::GetClassname() const
{
	static char* psz = "Baiter";
	return psz;
};
#endif




void Defcon::CBaiter::Move(float fTime)
{
	// The baiter basically has only one state: 
	// to chase the player.

	//m_fAge += fTime;
	//this->DoPhysics2(fTime);
	IGameObject* pTarget = m_pTarget;

	if(pTarget != nullptr)
	{
		const float xd = gpArena->HorzDistance(m_pos, pTarget->m_pos);
		float d = m_fDrag;
		float m = m_fMass;
		// Increase drag as baiter near player.
		float t = NORM_(xd, 0.0f, 500.0f);
		t = FMath::Min(1.0f, t);
		//m_fDrag = LERP(m_fDrag*30.0f, m_fDrag, t);
		m_fMass = LERP(m_fMass*3.0f, m_fMass, t);
		CEnemy::Move(fTime);
		m_fDrag = d;
		m_fMass = m;
	}
	//m_bIsCollisionInjurious = true;

	m_inertia = m_pos;

	// Don't move until after materialization is complete.
	if(m_fAge > ENEMY_BIRTHDURATION)
	{
		// Thrust in a way that will take us towards 
		// the player. Only thrust vertically if we 
		// are not within range.

		if(pTarget != nullptr)
		{
			CFPoint delta, target;
			target = pTarget->m_pos;
			const float vsign = m_bPreferTargetUnderside ? -1.0f : 1.0f;

			target += CFPoint(
				(float)cos(m_fAge) * 300.0f * SGN(pTarget->m_orient.fwd.x),
				vsign * (float)fabs(sin(m_fAge)) * 50.0f + vsign * 50.0f);

			float xd = gpArena->Direction(m_pos, target, delta);
			bool bMoveTowards = (xd > 150 || SGN(m_orient.fwd.x) != SGN(pTarget->m_orient.fwd.x));

			if(bMoveTowards)
			{
				int32 ctl = (delta.x < 0)	
						? ILiveGameObject::ctlBack
						: ILiveGameObject::ctlFwd;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack)
						? ILiveGameObject::ctlFwd
						: ILiveGameObject::ctlBack;

				if(this->NavControl_Duration(ctl) == 0)
					m_ctlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					m_ctlStartTime[ctl]);
				this->SetNavControl(ctl2, false, 0);
			}
			else
			{
				// Parallel the player.
				this->SetNavControl(
					ILiveGameObject::ctlBack, false, 0);
				this->SetNavControl(
					ILiveGameObject::ctlFwd, false, 0);
			}

			// Vertical.
			bMoveTowards = (xd > 50/* && SGN(m_orient.fwd.y) != SGN(pTarget->m_orient.fwd.y)*/);
			if(bMoveTowards)
			{
				int32 ctl = (delta.y < 0)	
						? ILiveGameObject::ctlDown
						: ILiveGameObject::ctlUp;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlDown)
						? ILiveGameObject::ctlUp
						: ILiveGameObject::ctlDown;

				if(this->NavControl_Duration(ctl) == 0)
					m_ctlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					m_ctlStartTime[ctl]);
				this->SetNavControl(ctl2, false, 0);
			}
			else
			{
				this->SetNavControl(
					ILiveGameObject::ctlDown, false, 0);
				this->SetNavControl(
					ILiveGameObject::ctlUp, false, 0);
			}

			// Fire control.
			if(FRAND <= 0.05f * 0.33f
				&& this->CanBeInjured()
				&& gpArena->IsPointVisible(m_pos))
				gpArena->FireBullet(*this, m_pos, 1, 1);

		}
	}

	m_inertia = m_pos - m_inertia;
}


void Defcon::CBaiter::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CBaiter::GetExplosionColorBase() const
{
	return CGameColors::yellow;
}
