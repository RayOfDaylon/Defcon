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
	ParentType = Type;
	Type = EObjType::BAITER;
	PointValue = BAITER_VALUE;
	RadarColor = MakeColorFromComponents(192, 192, 0);
	AnimSpeed = FRAND * 0.35f + 0.15f;

	Drag = PLAYER_DRAG * 0.15f;
	m_maxThrust = PLAYER_MAXTHRUST * 9.0f;
	Mass = PLAYER_MASS * 20;
	bIsCollisionInjurious = true;
	m_bPreferTargetUnderside = FRAND >= 0.5f;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
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

	//Age += fTime;
	//this->DoPhysics2(fTime);
	IGameObject* pTarget = m_pTarget;

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
	//bIsCollisionInjurious = true;

	Inertia = Position;

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
				(float)cos(Age) * 300.0f * SGN(pTarget->Orientation.fwd.x),
				vsign * (float)fabs(sin(Age)) * 50.0f + vsign * 50.0f);

			float xd = gpArena->Direction(Position, target, delta);
			bool bMoveTowards = (xd > 150 || SGN(Orientation.fwd.x) != SGN(pTarget->Orientation.fwd.x));

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
			bMoveTowards = (xd > 50/* && SGN(Orientation.fwd.y) != SGN(pTarget->Orientation.fwd.y)*/);
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
				&& gpArena->IsPointVisible(Position))
				gpArena->FireBullet(*this, Position, 1, 1);

		}
	}

	Inertia = Position - Inertia;
}


void Defcon::CBaiter::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CBaiter::GetExplosionColorBase() const
{
	return CGameColors::yellow;
}
