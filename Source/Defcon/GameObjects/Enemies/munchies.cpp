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
	ParentType = Type;
	Type = EObjType::PHRED;
	PointValue = PHRED_VALUE;
	RadarColor = MakeColorFromComponents(192, 128, 192);
	AnimSpeed = FRAND * 0.35f + 0.15f;

	Drag = PLAYER_DRAG * 0.15f;
	MaxThrust = PLAYER_MAXTHRUST * 9.0f;
	Mass = PLAYER_MASS * 20;
	bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	Brightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	//m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
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
	//Age += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = Position;

	IGameObject* pTarget = TargetPtr;
	if(pTarget != nullptr)
	{
		const float xd = gpArena->HorzDistance(Position, pTarget->Position);
		float d = Drag;
		float m = Mass;
		// Increase drag as baiter near player.
		float t = NORM_(xd, 0.0f, 500.0f);
		t = FMath::Min(1.0f, t);
		//Drag = LERP(Drag*30.0f, Drag, t);
		Mass = LERP(Mass*3.0f, Mass, t);
		CEnemy::Move(fTime);
		Drag = d;
		Mass = m;
	}

	orgpos -= Position;
	m_bFacingLeft = (orgpos.x > 0);
	Sprite->FlipHorizontal = (m_bFacingLeft);
	//bIsCollisionInjurious = true;


	// Don't move until after materialization is complete.
	if(Age > ENEMY_BIRTHDURATION)
	{
		// Thrust in a way that will take us towards 
		// the player. Only thrust vertically if we 
		// are not within range.

		if(pTarget != nullptr)
		{
			CFPoint delta, target;
			target = pTarget->Position;
			const float vsign = m_bPreferTargetUnderside ? -1.0f : 1.0f;

			target += CFPoint(
				(float)cos(Age) * 300.0f * SGN(pTarget->Orientation.Fwd.x),
				vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

			float xd = gpArena->Direction(Position, target, delta);
			bool bMoveTowards = (xd > 150 || SGN(Orientation.Fwd.x) != SGN(pTarget->Orientation.Fwd.x));
			if(bMoveTowards)
			{
				int32 ctl = (delta.x < 0)	
						? ILiveGameObject::ctlBack
						: ILiveGameObject::ctlFwd;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack)
						? ILiveGameObject::ctlFwd
						: ILiveGameObject::ctlBack;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
			bMoveTowards = (xd > 50/* && SGN(Orientation.Fwd.y) != SGN(pTarget->Orientation.Fwd.y)*/);
			if(bMoveTowards)
			{
				int32 ctl = (delta.y < 0)	
						? ILiveGameObject::ctlDown
						: ILiveGameObject::ctlUp;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlDown)
						? ILiveGameObject::ctlUp
						: ILiveGameObject::ctlDown;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
				&& gpArena->IsPointVisible(Position))
				gpArena->FireBullet(*this, Position, 1, 1);
*/

			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(phred);
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


Defcon::EColor Defcon::CPhred::GetExplosionColorBase() const
{
	return EColor::gray;
}


// -------------------------------------------------

Defcon::CMunchie::CMunchie()
{
	ParentType = Type;
	Type = EObjType::MUNCHIE;
	PointValue = MUNCHIE_VALUE;
	RadarColor = MakeColorFromComponents(0, 192, 0);
	AnimSpeed = FRAND * 0.35f + 0.15f;

	Drag = PLAYER_DRAG * 0.15f;
	MaxThrust = PLAYER_MAXTHRUST * 9.0f;
	Mass = PLAYER_MASS * 18;
	bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	Brightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	//m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
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
	//Age += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = Position;

	IGameObject* pTarget = TargetPtr;
	if(pTarget != nullptr)
	{
		const float xd = gpArena->HorzDistance(Position, pTarget->Position);
		float d = Drag;
		float m = Mass;
		// Increase drag as baiter near player.
		float t = NORM_(xd, 0.0f, 500.0f);
		t = FMath::Min(1.0f, t);
		//Drag = LERP(Drag*30.0f, Drag, t);
		Mass = LERP(Mass*3.0f, Mass, t);
		CEnemy::Move(fTime);
		Drag = d;
		Mass = m;
	}

	orgpos -= Position;
	m_bFacingLeft = (orgpos.x > 0);
	Sprite->FlipHorizontal = (m_bFacingLeft);
	//bIsCollisionInjurious = true;


	// Don't move until after materialization is complete.
	if(Age > ENEMY_BIRTHDURATION)
	{
		// Thrust in a way that will take us towards 
		// the player. Only thrust vertically if we 
		// are not within range.

		if(pTarget != nullptr)
		{
			CFPoint delta, target;
			target = pTarget->Position;
			const float vsign = m_bPreferTargetUnderside ? -1.0f : 1.0f;

			target += CFPoint(
				(float)cos(Age) * 300.0f * SGN(pTarget->Orientation.Fwd.x),
				vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

			float xd = gpArena->Direction(Position, target, delta);
			bool bMoveTowards = (xd > 150 || SGN(Orientation.Fwd.x) != SGN(pTarget->Orientation.Fwd.x));
			if(bMoveTowards)
			{
				int32 ctl = (delta.x < 0)	
						? ILiveGameObject::ctlBack
						: ILiveGameObject::ctlFwd;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack)
						? ILiveGameObject::ctlFwd
						: ILiveGameObject::ctlBack;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
			bMoveTowards = (xd > 50/* && SGN(Orientation.Fwd.y) != SGN(pTarget->Orientation.Fwd.y)*/);
			if(bMoveTowards)
			{
				int32 ctl = (delta.y < 0)	
						? ILiveGameObject::ctlDown
						: ILiveGameObject::ctlUp;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlDown)
						? ILiveGameObject::ctlUp
						: ILiveGameObject::ctlDown;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
				&& gpArena->IsPointVisible(Position))
				gpArena->FireBullet(*this, Position, 1, 1);
*/
			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(munchie);
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


Defcon::EColor Defcon::CMunchie::GetExplosionColorBase() const
{
	return EColor::green;
}


// -------------------------------------------------

Defcon::CBigRed::CBigRed()
{
	ParentType = Type;
	Type = EObjType::BIGRED;
	PointValue = BIGRED_VALUE;
	RadarColor = MakeColorFromComponents(255, 128, 0);
	AnimSpeed = FRAND * 0.35f + 0.15f;

	Drag = PLAYER_DRAG * 0.15f;
	MaxThrust = PLAYER_MAXTHRUST * 9.0f;
	Mass = PLAYER_MASS * 22;
	bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;
	Brightness = FRAND * 0.1f + 0.4f;
	m_fSquakTime = 0;
	//m_fBirthDuration = (FRAND * 0.25f + 0.25f) * ENEMY_BIRTHDURATION;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);
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
	//Age += fTime;
	//this->DoPhysics2(fTime);
	CFPoint orgpos = Position;

 	IGameObject* pTarget = TargetPtr;

	if(pTarget != nullptr)
	{
		const float xd = gpArena->HorzDistance(Position, pTarget->Position);
		float d = Drag;
		float m = Mass;
		// Increase drag as baiter near player.
		float t = NORM_(xd, 0.0f, 500.0f);
		t = FMath::Min(1.0f, t);
		//Drag = LERP(Drag*30.0f, Drag, t);
		Mass = LERP(Mass*3.0f, Mass, t);
		CEnemy::Move(fTime);
		Drag = d;
		Mass = m;
	}

	orgpos -= Position;
	m_bFacingLeft = (orgpos.x > 0);
	//bIsCollisionInjurious = true;
	Sprite->FlipHorizontal = (m_bFacingLeft);

	// Don't move until after materialization is complete.
	//if(Age > m_fBirthDuration)
	{
		// Thrust in a way that will take us towards 
		// the player. Only thrust vertically if we 
		// are not within range.

		if(pTarget != nullptr)
		{
			CFPoint delta, target;
			target = pTarget->Position;
			const float vsign = m_bPreferTargetUnderside ? -1.0f : 1.0f;

			target += CFPoint(
				(float)cos(Age) * 300.0f * SGN(pTarget->Orientation.Fwd.x),
				vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

			float xd = gpArena->Direction(Position, target, delta);
			bool bMoveTowards = (xd > 150 || SGN(Orientation.Fwd.x) != SGN(pTarget->Orientation.Fwd.x));
			if(bMoveTowards)
			{
				int32 ctl = (delta.x < 0)	
						? ILiveGameObject::ctlBack
						: ILiveGameObject::ctlFwd;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack)
						? ILiveGameObject::ctlFwd
						: ILiveGameObject::ctlBack;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
			bMoveTowards = (xd > 50/* && SGN(Orientation.Fwd.y) != SGN(pTarget->Orientation.Fwd.y)*/);
			if(bMoveTowards)
			{
				int32 ctl = (delta.y < 0)	
						? ILiveGameObject::ctlDown
						: ILiveGameObject::ctlUp;

				int32 ctl2 = (ctl ==  ILiveGameObject::ctlDown)
						? ILiveGameObject::ctlUp
						: ILiveGameObject::ctlDown;

				if(this->NavControl_Duration(ctl) == 0)
					ControlStartTime[ctl] = GameTime();
				this->SetNavControl(ctl, true, 
					ControlStartTime[ctl]);
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
				&& gpArena->IsPointVisible(Position))
				gpArena->FireBullet(*this, Position, 1, 1);
*/
			if(xd < 350 && FRAND < 0.02 && m_fSquakTime == 0.0f)
			{
				m_fSquakTime = 0.0001f;
				gpAudio->OutputSound(bigred);
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


Defcon::EColor Defcon::CBigRed::GetExplosionColorBase() const
{
	return EColor::red;
}
