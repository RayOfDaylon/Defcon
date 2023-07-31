// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	fireball.cpp
	Fireball entity for Defcon game.

	The fireball is shot (normally) by firebombers.
	It is a bit larger than a swarmer and looks 
	like a fluctuating orange/reddish hairball.
	Unlike a bullet, it can be shot down by the player.
	The movement routine pretends gravity exists, 
	so if we reach the bottom of the screen, 
	we're done.

	In Stargate, firebombers only shot fireballs.
*/

#include "fireball.h"


#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"




Defcon::CFireball::CFireball()
	:
	m_bFirstTime(true)
{
	ParentType = Type;
	Type = EObjType::FIREBALL;
	PointValue = FIREBALL_VALUE;
	//bInjurious = true;
	bCanBeInjured = true;

	RadarColor = C_ORANGE;
	Orientation.Fwd.y = -1.0f;
	m_fSpeed = 0; // the shooter sets the speed.
	// Depending on accuracy, the shooter takes gravity 
	// into account so that the fireball will hit the 
	// target. Again, the shot can be at the target's 
	// current position or be lead.

	//m_bMaterializes = false;
	bIsCollisionInjurious = true;

	CreateSprite(Type);

	//CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::fireball0);
	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


#ifdef _DEBUG
const char* Defcon::CFireball::GetClassname() const
{
	static char* psz = "Fireball";
	return psz;
}
#endif



void Defcon::CFireball::Move(float fTime)
{
	// Do first-time stuff.
	if(m_bFirstTime)
	{
		m_bFirstTime = false;
		// todo: We're aiming at the player, but in the future, 
		// we should generalize so that fireballs can be 
		// shot at anything.

		CPlayer* pTarget = &gpArena->GetPlayerShip();

		if(pTarget == nullptr)
		{
			this->MarkAsDead();
			return;
		}
		else
		{
			CFPoint dir;
			m_fSpeed = (FRAND * 0.5f + 1.0f) * gpArena->Direction(Position, pTarget->Position, dir);
			Orientation.Fwd.x = (float)(SGN(dir.x));
			Orientation.Fwd.y = (float)(SGN(dir.y));
		}
	}

	CEnemy::Move(fTime);
	Inertia = Position;

	// Self-destruct if we pass below ground.
	if(Position.y < 0)
	{
		this->MarkAsDead();
		return;
	}

	if(BULLETS_HIT_TERRAIN)
	{
		WRAP(Position.x, 0, gpArena->GetWidth());
		if(Position.y <= gpArena->GetTerrainElev(Position.x))
		{
			this->MarkAsDead();
			return;
		}
	}

		
	CFPoint motion(Orientation.Fwd);
	motion.x *= m_fSpeed * fTime;
	motion.y -= Age * Age;

	Position += motion;

	Inertia = Position - Inertia;
}



void Defcon::CFireball::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
#if 0
	int h = framebuf.GetHeight();

	CFPoint pt;
	mapper.To(Position, pt);

	CTrueBitmap& bmp = gBitmaps.GetBitmap(
		BRAND
		? CBitmaps::fireball0 + IRAND(12)
		: CBitmaps::bullet5x5 + IRAND(6));

	int w = bmp.GetWidth();
	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		pt.sub(CFPoint((float)w/2,
					(float)bmp.GetHeight()/2));
		bmp.BlitAlphaBrighten(
			framebuf, ROUND(pt.x), ROUND(pt.y), 
			w, bmp.GetHeight(), 
			0, 0, FRAND * 0.1f + 0.9f);
	}
#endif
}


int Defcon::CFireball::GetExplosionColorBase() const
{
	return CGameColors::red;
}


float Defcon::CFireball::GetExplosionMass() const
{
	return 0.33f;
}


void Defcon::CFireball::Explode(CGameObjectCollection& debris)
{
	const int cby = BRAND ? CGameColors::yellow : CGameColors::orange;

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = BRAND ? CGameColors::yellow : CGameColors::red;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 5;
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


