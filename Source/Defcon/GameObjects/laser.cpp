// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	laser.cpp
	Laserbeam weapon routines for Defcon game.
*/


#include "laser.h"

#include "Globals/prefs.h"
#include "Arenas/DefconPlayViewBase.h"


#define MAX_LASERAGE	0.5f


void Defcon::CLaserWeapon::Fire(CGameObjectCollection& goc)
{
	CLaserbeam* pBeam = new CLaserbeam;
	
	pBeam->SetCreatorType(m_pObject->GetType());

	CFPoint pos(m_pObject->m_pos);
	pos += m_emissionPt;

	pBeam->Create(pos, m_pObject->m_orient);
	pBeam->m_fArenawidth = m_fArenawidth;
	pBeam->m_pMapper     = m_pObject->m_pMapper;
	
	pBeam->SetMaxLength((int)(gpArena->GetDisplayWidth() * 0.75f));

	goc.Add(pBeam);
}

// -------------------------------------------------------

Defcon::CLaserbeam::CLaserbeam()
	:
	m_fLength(0)
{
	m_parentType = m_type;
	m_type = ObjType::LASERBEAM;
	m_bInjurious = true;
	m_bCanBeInjured = false;

	m_smallColor.A = 0.0f;
	m_color = MakeColorFromComponents(255, 0, 0);
	m_bMortal = true;
	m_fLifespan = MAX_LASERAGE;
	m_fScale =  (FRAND * 0.65f + 0.85f);
	m_fLifespan /= m_fScale;
	m_maxAge = m_fLifespan;
	m_fScale *= BEAM_MAXLEN;
}


void Defcon::CLaserbeam::SetMaxLength(int n)
{
	m_maxLength = n;
	m_fScale =  (FRAND * 0.65f + 0.85f);
	m_fScale *= m_maxLength;
}


#ifdef _DEBUG
const char* Defcon::CLaserbeam::GetClassname() const
{
	static char* psz = "Laserbeam";
	return psz;
}
#endif


void Defcon::CLaserbeam::Create(const CFPoint& where, const Orient2D& aim)
{
	m_pos = where;
	m_orient = aim;
	float budge = FRAND;
	budge -= 0.5f;
	budge *= 3;	
	m_pos.muladd(aim.up, budge);
}


void Defcon::CLaserbeam::Move(float fTime)
{
	// The length increases rapidly over time.
	if(m_fLifespan > 0)
	{
		m_fLength = m_maxAge - m_fLifespan;
		m_fLength /= m_maxAge; // 0..1

		float f = m_fLength; 
		m_posStart = m_pos;
		m_posStart.muladd(m_orient.fwd, f * m_fScale);
	}
}


void Defcon::CLaserbeam::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	// todo: make laserbeam draw using a random pick 
	// of beam textures. If we make the textures
	// grayscale, then we can use them as masks so 
	// that the color can still vary.

	float fAge = m_fLifespan / m_maxAge; // 0..1

	//int h = framebuf.GetHeight();

	CFPoint ptEnd(m_posStart);
	ptEnd.muladd(m_orient.fwd, m_fLength * m_fScale);

	CFPoint p1, p2;
	mapper.To(m_posStart, p1);
	mapper.To(ptEnd, p2);

	CFVector c1;
	if(FRAND > 0.125f)
	{
		c1.set(FRAND * .25f + .75f,
				FRAND,// * .25f + .75f;
				FRAND);
	}
	else
	{
		c1.set(1.0f, 1.0f, FRAND);
	}

	//c1.lerp(CFVector(1,1,1), fAge);
	c1.mul(255);

	float dist = p1.distance(p2);
	if(dist > framebuf.GetWidth() * 2)
	{
		return;
	}

#if 0
	framebuf.ColorStraightLine(
		(int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, 
		MakeColorFromComponents(c1.x, c1.y, c1.z));

	if(fAge > 0.5f)
	{
		framebuf.BrightenStraightLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		framebuf.BrightenStraightLine((int)p1.x, (int)p1.y+1, (int)p2.x, (int)p2.y+1);
	}
#else
	framebuf.DrawLaserBeam((int)p1.x, (int)p1.y, (int)p2.x, MakeColorFromComponents(c1.x, c1.y, c1.z));

	if(fAge > 0.5f)
	{
		framebuf.BrightenStraightLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		framebuf.BrightenStraightLine((int)p1.x, (int)p1.y+1, (int)p2.x, (int)p2.y+1);
	}
#endif
}


void Defcon::CLaserbeam::DrawSmall(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&)
{
}


void Defcon::CLaserbeam::GetInjurePt(CFPoint& pt) const
{
	pt = m_posStart;
	pt.muladd(m_orient.fwd, m_fLength * m_fScale);
}


bool Defcon::CLaserbeam::TestInjury(const CFRect& r) const
{
	CFPoint ptEnd = m_posStart;
	ptEnd.muladd(m_orient.fwd, m_fLength * m_fScale);

	if(m_posStart.y > r.LL.y && m_posStart.y < r.UR.y)
	{
		// The beam is vertically within the target.
		check(m_pMapper != nullptr);

		// Check hit in screen space.
		CFPoint beam1, beam2, target1, target2;
		m_pMapper->To(m_posStart, beam1);
		m_pMapper->To(ptEnd, beam2);
		m_pMapper->To(r.lowerleft(), target1);
		m_pMapper->To(r.upperright(), target2);

		float t;
		ORDER(beam1.x, beam2.x, t)
		ORDER(target1.x, target2.x, t)

		if(beam1.x < target2.x && beam2.x > target1.x)
		{
			return true;
		}
	}
	return false;
}
