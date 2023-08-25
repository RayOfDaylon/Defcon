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
	CLaserbeam* Beam = new CLaserbeam;
	
	Beam->SetCreatorType(Wielder->GetType());

	CFPoint P(Wielder->Position);
	P += m_emissionPt;

	Beam->InitLaserBeam(P, Wielder->Orientation, Wielder->MapperPtr);

	ObjectCollection.Add(Beam);
}

// -------------------------------------------------------

Defcon::CLaserbeam::CLaserbeam()
{
	ParentType = Type;
	Type       = EObjType::LASERBEAM;

	bInjurious    = true;
	bCanBeInjured = false;
	bMortal       = true;
	Lifespan      = 100.0f;
}



void Defcon::CLaserbeam::InitLaserBeam(const CFPoint& Where, const Orient2D& Aim, I2DCoordMapper* Mapper)
{
	MapperPtr   = Mapper;
	Position    = Where;
	Orientation = Aim;
	Position.MulAdd(Aim.Up, FRANDRANGE(-1.5f, 1.5f));

	StartX = Position.x;
	EndX   = StartX;
}


void Defcon::CLaserbeam::Tick(float DeltaTime)
{
	// We destroy ourselves when the EndX hits a screen edge.

	CFPoint P(EndX, Position.y);
	CFPoint ScreenP;

	MapperPtr->To(P, ScreenP);

	if(Orientation.Fwd.x < 0)
	{
		if(ScreenP.x <= 0)
		{
			Lifespan = 0;
			return;
		}
	}
	else if(ScreenP.x >= GArena->GetDisplayWidth())
	{
		Lifespan = 0;
		return;
	}

	StartX += LASER_SPEED.Low()  * DeltaTime * Orientation.Fwd.x;
	EndX   += LASER_SPEED.High() * DeltaTime * Orientation.Fwd.x;
}


void Defcon::CLaserbeam::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	// todo: the trailing half of the beam should look like it's disentegrating or fading out.

	const CFPoint StartP (StartX, Position.y), 
                  EndP   (EndX,   Position.y);

	CFPoint P1, P2;
	Mapper.To(StartP, P1);
	Mapper.To(EndP,   P2);

	// todo: this check is probably not necessary
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

	Painter.DrawLaserBeam(P1.x, P1.y, P2.x, Color);
}


void Defcon::CLaserbeam::DrawSmall(FPainter&, const I2DCoordMapper&, FSlateBrush&)
{
}


void Defcon::CLaserbeam::GetInjurePt(CFPoint& Pt) const
{
	Pt.Set(EndX, Position.y);
}


bool Defcon::CLaserbeam::TestInjury(const CFRect& R) const
{
	check(MapperPtr != nullptr);

	// No hit if beam is above or below the target.
	if(Position.y <= R.LL.y || Position.y >= R.UR.y)
	{
		return false;
	}

	CFPoint PtEnd;
	GetInjurePt(PtEnd);


	// We can't hit test x-values in world space because it will fail if 
	// the target is completely on the other side of the x-origin.

	CFPoint Beam1, Beam2, Target1, Target2;

	MapperPtr->To(CFPoint(StartX, Position.y),  Beam1);
	MapperPtr->To(PtEnd,                        Beam2);
	MapperPtr->To(R.LowerLeft(),                Target1);
	MapperPtr->To(R.UpperRight(),               Target2);

	Daylon::Order(Beam1.x, Beam2.x);
	Daylon::Order(Target1.x, Target2.x);

	return (Beam1.x < Target2.x && Beam2.x > Target1.x);
}
