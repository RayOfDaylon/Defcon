/*
	bullet.cpp
	Bullet weaponsfire routines for Defcon game.
	Copyright 2004 Daylon Graphics Ltd.
*/


#include "bullet.h"







#include "Globals/prefs.h"

#include "Arenas/DefconPlayViewBase.h"


// --------------------------------------------------------

#define MIN_BULLETAGE	2.5f
#define MAX_BULLETAGE	3.5f


Defcon::IBullet::IBullet()
{
	m_parentType = m_type;
	m_type = ObjType::BULLET;
	m_bInjurious = true;
	m_bCanBeInjured = false;

	m_smallColor.A = 0.0f;
	m_color = MakeColorFromComponents(255, 255, 255);
	m_bMortal = true;
	m_fOrgLifespan = m_fLifespan = FRANDRANGE(MIN_BULLETAGE, MAX_BULLETAGE);
	m_fRadius = 5;
	m_fSpeed = FRAND * 300 + 352;
}


void Defcon::IBullet::Move(float fTime)
{
	// Make the bullet flicker.
	if(Sprite)
	{
		Sprite->SetCurrentCel(IRAND(NumSpriteCels));
	}

	// Self-destruct if we pass out of the vertical bounds.
	if(m_pos.y < 0 || m_pos.y > gpArena->GetHeight())
	{
		this->MarkAsDead();
	}

	// todo: If the object processor is wrapping x-pos, why are we wrapping it here?
	// A baiter managed to emit a bullet that had a -xpos, I guess if it was near the origin
	// it could do that in a single frame, so if we use xpos here we have to wrap it.
	if(BULLETS_HIT_TERRAIN)
	{
		WRAP(m_pos.x, 0, gpArena->GetWidth());
		check(m_pos.x >= 0.0f && m_pos.x < gpArena->GetWidth());
		if(m_pos.y <= gpArena->GetTerrainElev(m_pos.x))
		{
			this->MarkAsDead();
		}
	}

	m_pos.muladd(m_orient.fwd, fTime * m_fSpeed);
}


void Defcon::IBullet::GetInjurePt(CFPoint& pt) const
{
	pt = m_pos;
}


bool Defcon::IBullet::TestInjury(const CFRect& r) const
{
	return r.ptinside(m_pos);
}

// ------------------------------------------------------------------

Defcon::CBullet::CBullet()
{
	m_parentType = m_type;
	m_type = ObjType::BULLET_ROUND;
	//m_eStyle = regular;

	CreateSprite(m_type);
	Sprite->IsStatic = true;

	NumSpriteCels = GameObjectResources.Get(m_type).Atlas->Atlas.NumCels;
}


#ifdef _DEBUG
const char* Defcon::CBullet::GetClassname() const
{
	static char* psz = "Bullet";
	return psz;
}
#endif

#if 0
void Defcon::CBullet::DrawBbox(FPaintArguments&, const I2DCoordMapper&)
{
}


void Defcon::CBullet::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
#if 0
	int h = framebuf.GetHeight();

	CFPoint pt;
	mapper.To(m_pos, pt);

	float t = m_fLifespan / m_fOrgLifespan;
	if(t > 0.25f)
		t = 1.0f;
	else
		t *= 4;


	CTrueBitmap& bmp = gBitmaps.GetBitmap(
		m_eStyle == regular
		? CBitmaps::bullet5x5 + IRAND(6)
		: CBitmaps::bulletthin + BRAND);
	int w = bmp.GetWidth();
	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		pt.sub(CFPoint((float)w/2,
					(float)bmp.GetHeight()/2));
		bmp.BlitAlphaBrighten(
			framebuf, ROUND(pt.x), ROUND(pt.y), 
			w, bmp.GetHeight(), 
			0, 0, t);
	}
#endif
}
#endif

// -------------------------------------------------------------------------

Defcon::CThinBullet::CThinBullet()
{
	m_parentType = m_type;
	m_type = ObjType::BULLET_THIN;

	CreateSprite(m_type);
	Sprite->IsStatic = true;

	NumSpriteCels = GameObjectResources.Get(m_type).Atlas->Atlas.NumCels;
}


#ifdef _DEBUG
const char* Defcon::CBullet::GetClassname() const
{
	static char* psz = "ThinBullet";
	return psz;
}
#endif


