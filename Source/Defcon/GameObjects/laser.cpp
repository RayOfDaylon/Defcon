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

	CreateSprite(Type);
	InstallSprite();
	Sprite->Hide();

	// todo: colorize the sprite. Either add a tint, or use more atlases.
}


void Defcon::CLaserbeam::InitLaserBeam(const CFPoint& Where, const Orient2D& Aim, I2DCoordMapper* Mapper)
{
	MapperPtr   = Mapper;
	Position    = Where;
	Orientation = Aim;
	Position.MulAdd(Aim.Up, FRANDRANGE(-1.5f, 1.5f));

	StartX = Position.x;
	EndX   = StartX;

	Sprite->FlipHorizontal = (Orientation.Fwd.x < 0);
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
			Sprite->Hide();
			return;
		}
	}
	else if(ScreenP.x >= GArena->GetDisplayWidth())
	{
		Lifespan = 0;
		Sprite->Hide();
		return;
	}

	// Only show the sprite when it has valid geometry. Otherwise
	// we can get a frame where the beam appears in the wrong place.

	Sprite->Show();

	StartX += LASER_SPEED.Low()  * DeltaTime * Orientation.Fwd.x;
	EndX   += LASER_SPEED.High() * DeltaTime * Orientation.Fwd.x;

	// Sprites are centered, so we have to shift the x-pos accordingly.

	Position.x = AVG(EndX, StartX);

	Sprite->SetSize(FVector2D(ABS(EndX - StartX), 2));
	Sprite->UpdateWidgetSize();
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
