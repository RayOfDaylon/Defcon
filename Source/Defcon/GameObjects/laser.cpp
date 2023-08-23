// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	laser.cpp
	Laserbeam weapon routines for Defcon game.
*/


#include "laser.h"
#include "Globals/prefs.h"
#include "Arenas/DefconPlayViewBase.h"


void Defcon::CLaserWeapon::Fire(CGameObjectCollection& ObjectCollection)
{
	CLaserbeam* BeamPtr = new CLaserbeam;
	
	BeamPtr->SetCreatorType(m_pObject->GetType());

	CFPoint P(m_pObject->Position);
	P += m_emissionPt;

	BeamPtr->Create(P, m_pObject->Orientation);
	//BeamPtr->m_fArenawidth = m_fArenawidth;
	BeamPtr->MapperPtr = m_pObject->MapperPtr;
	
	BeamPtr->SetMaxLength(GArena->GetDisplayWidth() * 0.75f);

	ObjectCollection.Add(BeamPtr);
}

// -------------------------------------------------------

Defcon::CLaserbeam::CLaserbeam()
	:
	Length(0)
{
	ParentType = Type;
	Type = EObjType::LASERBEAM;

	bInjurious = true;
	bCanBeInjured = false;
	//Color = MakeColorFromComponents(255, 0, 0);
	bMortal = true;
	Lifespan = LASER_AGE_MAX;
	Scale =  FRANDRANGE(0.85f, 1.5f);// (FRAND * 0.65f + 0.85f);
	Lifespan /= Scale;
	MaxAge = Lifespan;
	Scale *= BEAM_MAXLEN;
}


void Defcon::CLaserbeam::SetMaxLength(float Len)
{
	MaxLength = Len;
	Scale =  FRANDRANGE(0.85f, 1.5f);// (FRAND * 0.65f + 0.85f);
	Scale *= MaxLength;
}


#ifdef _DEBUG
const char* Defcon::CLaserbeam::GetClassname() const
{
	static char* psz = "Laserbeam";
	return psz;
}
#endif


void Defcon::CLaserbeam::Create(const CFPoint& Where, const Orient2D& Aim)
{
	Position = Where;
	Orientation = Aim;
	//float budge = FRAND;
	//budge -= 0.5f;
	//budge *= 3;	
	const float Budge = FRANDRANGE(-1.5f, 1.5f);
	Position.MulAdd(Aim.Up, Budge);
}


void Defcon::CLaserbeam::Tick(float DeltaTime)
{
	// The length increases rapidly over time.
	if(Lifespan > 0)
	{
		//Length = 1.0f - NORM_(Lifespan, 0.0f, MaxAge);
		Length = (MaxAge - Lifespan) / MaxAge;

		StartPosition = Position;
		StartPosition.MulAdd(Orientation.Fwd, Length * Scale);
	}
}


void Defcon::CLaserbeam::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	// todo: make laserbeam draw using a random pick 
	// of beam textures. If we make the textures
	// grayscale, then we can use them as masks so 
	// that the color can still vary.

	const float BeamAge = Lifespan / MaxAge; // 0..1

	CFPoint PtEnd(StartPosition);
	PtEnd.MulAdd(Orientation.Fwd, Length * Scale);

	CFPoint P1, P2;
	Mapper.To(StartPosition, P1);
	Mapper.To(PtEnd, P2);

	if(P1.Distance(P2) > Painter.GetWidth() * 2)
	{
		return;
	}

	FLinearColor Color;

	if(FRAND > 0.125f)
	{
		Color.R = FRANDRANGE(0.25f, 1.0f);
		Color.G = FRAND;
		Color.B = FRAND;
	}
	else
	{
		Color.R = 
		Color.G = 1.0f;
		Color.B = FRAND;
	}

	Color.A = 1.0f;

	//c1.lerp(CFVector(1,1,1), BeamAge);
	//c1.Mul(255);


#if 0
	Painter.ColorStraightLine(
		(int32)P1.x, (int32)P1.y, (int32)P2.x, (int32)P2.y, 
		MakeColorFromComponents(c1.x, c1.y, c1.z));

	if(BeamAge > 0.5f)
	{
		Painter.BrightenStraightLine((int32)P1.x, (int32)P1.y, (int32)P2.x, (int32)P2.y);
		Painter.BrightenStraightLine((int32)P1.x, (int32)P1.y+1, (int32)P2.x, (int32)P2.y+1);
	}
#else
	Painter.DrawLaserBeam(P1.x, P1.y, P2.x, Color);

#if 0
	// todo: BrightenStraightLine is a no-op currently 
	if(BeamAge > 0.5f)
	{
		Painter.BrightenStraightLine((int32)P1.x, (int32)P1.y,     (int32)P2.x, (int32)P2.y);
		Painter.BrightenStraightLine((int32)P1.x, (int32)P1.y + 1, (int32)P2.x, (int32)P2.y + 1);
	}
#endif
#endif
}


void Defcon::CLaserbeam::DrawSmall(FPainter&, const I2DCoordMapper&, FSlateBrush&)
{
}


void Defcon::CLaserbeam::GetInjurePt(CFPoint& Pt) const
{
	Pt = StartPosition;
	Pt.MulAdd(Orientation.Fwd, Length * Scale);
}


bool Defcon::CLaserbeam::TestInjury(const CFRect& R) const
{
	CFPoint PtEnd = StartPosition;
	PtEnd.MulAdd(Orientation.Fwd, Length * Scale);

	// No hit if beam is above or below the target.
	if(StartPosition.y <= R.LL.y || StartPosition.y >= R.UR.y)
	{
		return false;
	}

	check(MapperPtr != nullptr);

	// Check hit in screen space.
	CFPoint Beam1, Beam2, Target1, Target2;

	MapperPtr->To(StartPosition, Beam1);
	MapperPtr->To(PtEnd, Beam2);
	MapperPtr->To(R.LowerLeft(), Target1);
	MapperPtr->To(R.UpperRight(), Target2);

	float T;
	ORDER(Beam1.x, Beam2.x, T)
	ORDER(Target1.x, Target2.x, T)

	return (Beam1.x < Target2.x && Beam2.x > Target1.x);
}
