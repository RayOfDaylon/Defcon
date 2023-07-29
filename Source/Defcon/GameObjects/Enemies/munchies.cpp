// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	munchies.cpp
	Phred, Big Red, and Munchie enemies for Defcon game.

	These guys look like squarish PacMen and mimic
	baiters in arrival and movement, except they don't fire;
	collision is their goal. 
*/

#include "munchies.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CPhred::CPhred()
{
	m_parentType = m_type;
	m_type = ObjType::PHRED;
	m_pointValue = PHRED_VALUE;
	m_smallColor = MakeColorFromComponents(192, 128, 192);
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;

	m_fDrag = PLAYER_DRAG * 0.15f;
	m_maxThrust = PLAYER_MAXTHRUST * 9.0f;
	m_fMass = PLAYER_MASS * 20;
	m_bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	m_fBrightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CPhred::~CPhred()
{
}


#ifdef _DEBUG
const char* Defcon::CPhred::GetClassname() const
{
	static char* psz = "Phred";
	return psz;
};
#endif


void Defcon::CPhred::Move(float fTime)
{
	//m_fAge += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = m_pos;

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

	orgpos -= m_pos;
	m_bFacingLeft = (orgpos.x > 0);
	Sprite->FlipHorizontal = (m_bFacingLeft);
	//m_bIsCollisionInjurious = true;


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

/*			// Fire control.
			if(FRAND <= 0.05f * 0.33f
				&& this->CanBeInjured()
				&& gpArena->IsPointVisible(m_pos))
				gpArena->FireBullet(*this, m_pos, 1, 1);
*/

			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(snd_phred);
			}
			if(m_fSquakTime != 0.0f)
			{
				m_fSquakTime += fTime;
				if(m_fSquakTime > 1.0f)
					m_fSquakTime = 0.0f;
			}

		}
	}
}


void Defcon::CPhred::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CPhred::GetExplosionColorBase() const
{
	return CGameColors::gray;
}


// -------------------------------------------------

Defcon::CMunchie::CMunchie()
{
	m_parentType = m_type;
	m_type = ObjType::MUNCHIE;
	m_pointValue = MUNCHIE_VALUE;
	m_smallColor = MakeColorFromComponents(0, 192, 0);
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;

	m_fDrag = PLAYER_DRAG * 0.15f;
	m_maxThrust = PLAYER_MAXTHRUST * 9.0f;
	m_fMass = PLAYER_MASS * 18;
	m_bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	m_fBrightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CMunchie::~CMunchie()
{
}


#ifdef _DEBUG
const char* Defcon::CMunchie::GetClassname() const
{
	static char* psz = "Munchie";
	return psz;
};
#endif


void Defcon::CMunchie::Move(float fTime)
{
	//m_fAge += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = m_pos;

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

	orgpos -= m_pos;
	m_bFacingLeft = (orgpos.x > 0);
	Sprite->FlipHorizontal = (m_bFacingLeft);
	//m_bIsCollisionInjurious = true;


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

/*			// Fire control.
			if(FRAND <= 0.05f * 0.33f
				&& this->CanBeInjured()
				&& gpArena->IsPointVisible(m_pos))
				gpArena->FireBullet(*this, m_pos, 1, 1);
*/
			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(snd_munchie);
			}
			if(m_fSquakTime != 0.0f)
			{
				m_fSquakTime += fTime;
				if(m_fSquakTime > 1.0f)
					m_fSquakTime = 0.0f;
			}
		}
	}

}


void Defcon::CMunchie::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CMunchie::GetExplosionColorBase() const
{
	return CGameColors::green;
}


// -------------------------------------------------

Defcon::CBigRed::CBigRed()
{
	m_parentType = m_type;
	m_type = ObjType::BIGRED;
	m_pointValue = BIGRED_VALUE;
	m_smallColor = MakeColorFromComponents(255, 128, 0);
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;

	m_fDrag = PLAYER_DRAG * 0.15f;
	m_maxThrust = PLAYER_MAXTHRUST * 9.0f;
	m_fMass = PLAYER_MASS * 22;
	m_bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	m_fBrightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
}


Defcon::CBigRed::~CBigRed()
{
}


#ifdef _DEBUG
const char* Defcon::CBigRed::GetClassname() const
{
	static char* psz = "Big_Red";
	return psz;
};
#endif


void Defcon::CBigRed::Move(float fTime)
{
	//m_fAge += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = m_pos;

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

	orgpos -= m_pos;
	m_bFacingLeft = (orgpos.x > 0);
	//m_bIsCollisionInjurious = true;
	Sprite->FlipHorizontal = (m_bFacingLeft);

	// Don't move until after materialization is complete.
	if(m_fAge > m_fBirthDuration)
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

/*			// Fire control.
			if(FRAND <= 0.05f * 0.33f
				&& this->CanBeInjured()
				&& gpArena->IsPointVisible(m_pos))
				gpArena->FireBullet(*this, m_pos, 1, 1);
*/
			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(snd_bigred);
			}
			if(m_fSquakTime != 0.0f)
			{
				m_fSquakTime += fTime;
				if(m_fSquakTime > 1.0f)
					m_fSquakTime = 0.0f;
			}
		}
	}

}


void Defcon::CBigRed::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CBigRed::GetExplosionColorBase() const
{
	return CGameColors::red;
}
