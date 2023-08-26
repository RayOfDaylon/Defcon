// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	laser.cpp
	Laserbeam weapon routines for Defcon game.
*/


#include "laser.h"
#include "Globals/prefs.h"
#include "Common/util_color.h"
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
	Sprite->SetSize(FVector2D(0));
	Sprite->UpdateWidgetSize();
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

	CFPoint P;
	MapperPtr->To(Position, P);

	const float MaxBeamLength = Orientation.Fwd.x < 0 ? P.x : GArena->GetDisplayWidth() - P.x;
	EstimatedLifetime = MaxBeamLength / LASER_SPEED.High(); // at 5000 px/sec = 0.384, or 23 frames at 60 fps
}


void Defcon::CLaserbeam::Tick(float DeltaTime)
{
	// We destroy ourselves when the EndX hits a screen edge.

	CFPoint P(EndX, Position.y);
	CFPoint ScreenP;

	MapperPtr->To(P, ScreenP);

	if(ScreenP.x <= 0 || ScreenP.x >= GArena->GetDisplayWidth())
	{
		Lifespan = 0;
		return;
	}


	StartX += LASER_SPEED.Low()  * DeltaTime * Orientation.Fwd.x;
	EndX   += LASER_SPEED.High() * DeltaTime * Orientation.Fwd.x;

	// Sprites are centered, so we have to shift the x-pos accordingly.

	Position.x = AVG(EndX, StartX);

	const float BeamLength = ABS(EndX - StartX);
	Sprite->SetSize(FVector2D(BeamLength, 2));
	Sprite->UpdateWidgetSize();

	// Set current color.

	static const FLinearColor Colors[] =
	{
		C_WHITE,
		C_WHITE,
		C_WHITE,
		C_WHITE,
		C_LIGHTYELLOW,
		C_LIGHTYELLOW,
		C_LIGHTYELLOW,
		C_YELLOW,
		C_YELLOW,
		C_LIGHTORANGE,
		C_ORANGE
	};

	const int32 ColorIdx = FMath::Min(ROUND((Age / EstimatedLifetime) * (array_size(Colors) - 1)), (int32) array_size(Colors) - 1);

	Sprite->SetTint(Colors[ColorIdx]);

	Age += DeltaTime;
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
