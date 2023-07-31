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

	CFPoint pos(m_pObject->Position);
	pos += m_emissionPt;

	pBeam->Create(pos, m_pObject->Orientation);
	pBeam->m_fArenawidth = m_fArenawidth;
	pBeam->MapperPtr     = m_pObject->MapperPtr;
	
	pBeam->SetMaxLength((int)(gpArena->GetDisplayWidth() * 0.75f));

	goc.Add(pBeam);
}

// -------------------------------------------------------

Defcon::CLaserbeam::CLaserbeam()
	:
	m_fLength(0)
{
	ParentType = Type;
	Type = EObjType::LASERBEAM;
	bInjurious = true;
	bCanBeInjured = false;

	RadarColor.A = 0.0f;
	m_color = MakeColorFromComponents(255, 0, 0);
	bMortal = true;
	Lifespan = MAX_LASERAGE;
	m_fScale =  (FRAND * 0.65f + 0.85f);
	Lifespan /= m_fScale;
	m_maxAge = Lifespan;
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
	Position = where;
	Orientation = aim;
	float budge = FRAND;
	budge -= 0.5f;
	budge *= 3;	
	Position.MulAdd(aim.up, budge);
}


void Defcon::CLaserbeam::Move(float fTime)
{
	// The length increases rapidly over time.
	if(Lifespan > 0)
	{
		m_fLength = m_maxAge - Lifespan;
		m_fLength /= m_maxAge; // 0..1

		float f = m_fLength; 
		m_posStart = Position;
		m_posStart.MulAdd(Orientation.fwd, f * m_fScale);
	}
}


void Defcon::CLaserbeam::Draw(FPaintArguments& PaintArguments, const I2DCoordMapper& Mapper)
{
	// todo: make laserbeam draw using a random pick 
	// of beam textures. If we make the textures
	// grayscale, then we can use them as masks so 
	// that the color can still vary.

	const float fAge = Lifespan / m_maxAge; // 0..1

	CFPoint ptEnd(m_posStart);
	ptEnd.MulAdd(Orientation.fwd, m_fLength * m_fScale);

	CFPoint p1, p2;
	Mapper.To(m_posStart, p1);
	Mapper.To(ptEnd, p2);

	float dist = p1.Distance(p2);
	if(dist > PaintArguments.GetWidth() * 2)
	{
		return;
	}

	FLinearColor Color;

	//CFVector c1;

	if(FRAND > 0.125f)
	{
		//c1.Set(FRANDRANGE(0.25f, 1.0f), FRAND, FRAND);
		Color.R = FRANDRANGE(0.25f, 1.0f);
		Color.G = FRAND;
		Color.B = FRAND;
	}
	else
	{
		//c1.Set(1.0f, 1.0f, FRAND);
		Color.R = 
		Color.G = 1.0f;
		Color.B = FRAND;
	}

	Color.A = 1.0f;

	//c1.lerp(CFVector(1,1,1), fAge);
	//c1.Mul(255);


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
	PaintArguments.DrawLaserBeam((int)p1.x, (int)p1.y, (int)p2.x, Color /*MakeColorFromComponents(c1.x, c1.y, c1.z)*/);

	if(fAge > 0.5f)
	{
		PaintArguments.BrightenStraightLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		PaintArguments.BrightenStraightLine((int)p1.x, (int)p1.y+1, (int)p2.x, (int)p2.y+1);
	}
#endif
}


void Defcon::CLaserbeam::DrawSmall(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&)
{
}


void Defcon::CLaserbeam::GetInjurePt(CFPoint& pt) const
{
	pt = m_posStart;
	pt.MulAdd(Orientation.fwd, m_fLength * m_fScale);
}


bool Defcon::CLaserbeam::TestInjury(const CFRect& r) const
{
	CFPoint ptEnd = m_posStart;
	ptEnd.MulAdd(Orientation.fwd, m_fLength * m_fScale);

	// No hit if beam is above or below the target.
	if(m_posStart.y <= r.LL.y || m_posStart.y >= r.UR.y)
	{
		return false;
	}

	check(MapperPtr != nullptr);

	// Check hit in screen space.
	CFPoint beam1, beam2, target1, target2;
	MapperPtr->To(m_posStart, beam1);
	MapperPtr->To(ptEnd, beam2);
	MapperPtr->To(r.LowerLeft(), target1);
	MapperPtr->To(r.UpperRight(), target2);

	float t;
	ORDER(beam1.x, beam2.x, t)
	ORDER(target1.x, target2.x, t)

	return (beam1.x < target2.x && beam2.x > target1.x);
}
