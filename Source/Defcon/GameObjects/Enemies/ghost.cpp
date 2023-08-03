// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	ghost.cpp
	Ghost enemy type for Defcon game.
*/


#include "ghost.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"

#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


Defcon::CGhost::CGhost()
{
	ParentType = Type;
	Type       = EObjType::GHOST;

	PointValue = GHOST_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = MakeColorFromComponents(192, 192, 192);

	AnimSpeed     = FRANDRANGE(0.5f, 1.0f);
	
	NumParts           = IRAND(4) + 4;
	SpinVelocity       = FRANDRANGE(-1.0f, 1.0f);
	SpinAngle          = FRAND;
	SpinDir            = BRAND ? 1: -1;
	DispersalCountdown = 0.0f;
	FiringCountdown    = FRANDRANGE(2.0f, 4.0f);

	for(int I = 1; I < NumParts; I++)
	{
		PartRadiiMin[I] = FRANDRANGE(0.5f, 1.0f);
		PartRadiiMax[I] = FRANDRANGE(PartRadiiMin[I], 1.5f);
		PartRadiiSpeed[I] = FRANDRANGE(1.0f, 2.0f);
	}

	const auto& Info = GameObjectResources.Get(EObjType::GHOSTPART);
	BboxRadius = Info.Size * 0.5f; // seems too small, yet... here we are
}


Defcon::CGhost::~CGhost()
{
}

#ifdef _DEBUG
const char* Defcon::CGhost::GetClassname() const
{
	static char* psz = "Ghost";
	return psz;
};
#endif



void Defcon::CGhost::ConsiderFiringBullet(float DeltaTime)
{
	if(!gpArena->IsPointVisible(Position) || TargetPtr == nullptr)
	{
		return;
	}
		
	// Hold fire if target is below ground
	if(TargetPtr->Position.y < gpArena->GetTerrainElev(TargetPtr->Position.x))
	{
		return;
	}

	FiringCountdown -= DeltaTime;

	if(FiringCountdown <= 0.0f)
	{
		(void) gpArena->FireBullet(*this, Position, 1, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)gDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(3.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 1.0f);
	}
}


void Defcon::CGhost::Move(float DeltaTime)
{
	// Just float around drifting horizontally.

	if(DispersalCountdown > 0.0f)
	{
		DispersalCountdown -= DeltaTime;
		return;
	}

	// We are in normal form (not dispersed).

	CEnemy::Move(DeltaTime);

	Inertia = Position;

	Orientation.Fwd.y = 0.1f * (float)sin(Frequency * (OffsetY + Age)); 

	ConsiderFiringBullet(DeltaTime);

	//SpinVelocity = 1.0f;//sin(Age * PI * m_fSpinVelMax);
	SpinAngle += (SpinVelocity * DeltaTime);

	Position.MulAdd(Orientation.Fwd, DeltaTime * 50.0f);
	Inertia = Position - Inertia;

	// See if we need to disperse (player ship got too close).

	if(TargetPtr != nullptr && !this->MarkedForDeath())
	{
		CFPoint Direction;
		const float Distance = gpArena->ShortestDirection(Position, TargetPtr->Position, Direction);

		if(Distance < GHOST_PLAYER_DIST_MIN)
		{
			const float FlightTime = FRANDRANGE(0.5f, 1.5f);

			DispersalCountdown = FlightTime;

			// We don't have a sprite to hide, and our Draw method takes care of "hiding" us by
			// not rendering if we're dispersed (since the parts will render).
			//Sprite->Hide();

			// While hidden, move us to our reformation spot.
			CFPoint NewPos;

			// Choose somewhere else on the screen.
			NewPos.x = Position.x + SFRAND * gpArena->GetDisplayWidth() / 1.5f;
			NewPos.y = FRANDRANGE(0.125f, 0.875f) * gpArena->GetHeight();

			// Don't modulate newloc; it will cause wrong path animation if path cross x-origin.
			// Modulation must happen in ghostpart::move.

			for(int32 i = 1; i < NumParts; i++)
			{
				auto GhostPart = (CGhostPart*)gpArena->CreateEnemyNow(EObjType::GHOSTPART, PartLocs[i], EObjectCreationFlags::EnemyPart);

				GhostPart->SetCollisionInjurious(false);
				GhostPart->SetFlightDuration(FlightTime);
				GhostPart->SetFlightPath(Position, NewPos);
			}

			// Now move ourselves. Do this last because we use our original Position as the 
			// source position of the dispersal in the above loop.

			Position   = NewPos;
			Position.x = gpArena->WrapX(Position.x);
		
			gpAudio->OutputSound(EAudioTrack::Ghostflight);
		}
	}
}


void Defcon::CGhost::Draw(FPainter& Painter, const I2DCoordMapper& mapper)
{
	if(DispersalCountdown > 0.0f)
	{
		return;
	}

	PartLocs[0] = Position;

	const float F = (float)fmod(Age, AnimSpeed) / AnimSpeed;
		//(float)Age / AnimSpeed;


	// Draw the parts in a circle around a central part.
	const int32 N = NumParts - 1;

	const float R = BboxRadius.x * 1.25f;

	for(int32 I = 0; I < N; I++)
	{
		const float T = SpinDir * (float)(TWO_PI * (float)I / N + ((SpinAngle + PSIN(Age)/*FRAND * 0.1f*/) * TWO_PI));
		CFPoint Pt2((float)cos(T), (float)sin(T));
		const float Radius = (float)(sin((F /*+ FRAND * 3*/) * PI) * R * LERP(PartRadiiMin[I + 1], PartRadiiMax[I + 1], PSIN(Age * PartRadiiSpeed[I + 1])) /*+ 5.0f*/);

		Pt2 *= Radius;
		Pt2 += Position;
		PartLocs[I + 1] = Pt2;
	}

	for(int32 I = 0; I < NumParts; I++)
	{
		CFPoint pt;
		mapper.To(PartLocs[I], pt);
		this->DrawPart(Painter, pt);
	}
}


void Defcon::CGhost::DrawPart(FPainter& Painter, const CFPoint& Pt)
{
	auto& Info = GameObjectResources.Get(EObjType::GHOSTPART);

	const int W = Info.Size.X;

	if(Pt.x >= -W && Pt.x <= Painter.GetWidth() + W)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float F = (NumCels - 1) * PSIN(PI * fmod(Age, AnimSpeed) / AnimSpeed);

		const float Usize = 1.0f / NumCels;

		const auto S = Info.Size;
		const FSlateLayoutTransform Translation(FVector2D(Pt.x, Pt.y) - S / 2);
		const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

		const float F2 = Usize * ROUND(F);
		FBox2f UVRegion(FVector2f(F2, 0.0f), FVector2f(F2 + Usize, 1.0f));
		Info.Atlas->Atlas.AtlasBrush.SetUVRegion(UVRegion);

		FSlateDrawElement::MakeBox(
			*Painter.OutDrawElements,
			Painter.LayerId,
			Geometry.ToPaintGeometry(),
			&Info.Atlas->Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			C_WHITE * Painter.RenderOpacity);
	}
}


void Defcon::CGhost::Explode(CGameObjectCollection& Debris)
{
	//CEnemy::Explode(Debris);

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	// Create an explosion by making
	// several Debris objects and 
	// adding them to the Debris set.
	int32 N = (int32)(FRAND * 30 + 30);
	float MaxSize = FRAND * 5 + 3;

/*
	// Don'T use huge particles ever; it just looks silly in the end.
	if(FRAND < .08)
		MaxSize = 14;
*/

	// Define which color to make the Debris.
	auto ColorBase = this->GetExplosionColorBase();
	MaxSize *= this->GetExplosionMass();
	
	if(IRAND(3) == 1)
	{
		ColorBase = EColor::Gray;
	}

	bool bDieOff = (FRAND >= 0.25f);
	int32 I;

	float BrightBase;
	IGameObject* pFireblast = this->CreateFireblast(Debris, BrightBase);

	for(I = 0; I < N; I++)
	{
		CGlowingFlak* FlakPtr = new CGlowingFlak;

		FlakPtr->ColorbaseYoung = ColorBase;
		FlakPtr->LargestSize = MaxSize;
		FlakPtr->bFade = bDieOff;

		FlakPtr->Position = Position;
		FlakPtr->Orientation = Orientation;

		CFPoint Direction;
		double T = FRAND * TWO_PI;
		
		Direction.Set((float)cos(T), (float)sin(T));

		// Debris has at least the object's momentum.
		FlakPtr->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		FlakPtr->Orientation.Fwd *= FRANDRANGE(30, 42) * 1.5f;

		// Make the particle have a velocity vector
		// as if it were standing still.
		const float Speed = FRANDRANGE(90, 270);

		FlakPtr->Orientation.Fwd.MulAdd(Direction, Speed);

		Debris.Add(FlakPtr);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		N = (int32) FRANDRANGE(20, 40);
		MaxSize = FRAND * 4 + 8.0f;
		//MaxSize = FMath::Min(MaxSize, 9.0f);

		if(IRAND(3) == 1)
		{
			ColorBase = EColor::Gray;
		}
		else
		{
			ColorBase = this->GetExplosionColorBase();
		}

		for(I = 0; I < N; I++)
		{
			CGlowingFlak* FlakPtr = new CGlowingFlak;

			FlakPtr->ColorbaseYoung = ColorBase;
			FlakPtr->LargestSize = MaxSize;
			FlakPtr->bFade = bDieOff;

			FlakPtr->Position = Position;
			FlakPtr->Orientation = Orientation;

			CFPoint Direction;
			double T = FRAND * TWO_PI;
			
			Direction.Set((float)cos(T), (float)sin(T));

			FlakPtr->Orientation.Fwd = Inertia;
			FlakPtr->Orientation.Fwd *= FRANDRANGE(30, 42) * 1.5f;

			const float Speed = FRANDRANGE(22, 67);

			FlakPtr->Orientation.Fwd.MulAdd(Direction, Speed);

			Debris.Add(FlakPtr);
		}
	}


#if 1
	ColorBase = EColor::Gray;


	for(I = 0; I < 20; I++)
	{
		CFlak* FlakPtr = new CFlak;

		FlakPtr->ColorbaseYoung = ColorBase;
		FlakPtr->ColorbaseOld = ColorBase;
		FlakPtr->bCold = true;
		FlakPtr->LargestSize = 8;
		FlakPtr->bFade = true;//bDieOff;

		FlakPtr->Position = Position;
		FlakPtr->Orientation = Orientation;

		CFPoint Direction;
		double T = FRAND * TWO_PI;
		
		Direction.Set((float)cos(T), (float)sin(T));

		// Debris has at least the object's momentum.
		FlakPtr->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
			FlakPtr->Orientation.Fwd *= FRANDRANGE(20, 32) * 1.5f;
		//ndir *= FRAND * 0.4f + 0.2f;
		const float Speed = FRANDRANGE(110, 140);

		FlakPtr->Orientation.Fwd.MulAdd(Direction, Speed);

		Debris.Add(FlakPtr);
	}
#endif
}

// --------------------------------------------------------


Defcon::CGhostPart::CGhostPart()
{
	ParentType = Type;
	Type       = EObjType::GHOSTPART;

	PointValue = 0;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_WHITE;
	AnimSpeed = FRANDRANGE(0.15f, 0.5f);
	bCanBeInjured = false;
	bIsCollisionInjurious = false;

	CreateSprite(Type);
	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::CGhostPart::~CGhostPart()
{
}


#ifdef _DEBUG
const char* Defcon::CGhostPart::GetClassname() const
{
	static char* psz = "Ghost_part";
	return psz;
};
#endif


void Defcon::CGhostPart::SetFlightPath(const CFPoint& From, const CFPoint& To)
{
	// from and to must be unmodulated.

	Path.Pts[0] = Path.Pts[1] = From;
	Path.Pts[2] = Path.Pts[3] = To;

	// Make the control points match their anchor but with a random offset.
	Path.Pts[1] += CFPoint(FRANDRANGE(-300.0f, 300.0f), FRANDRANGE(-300.0f, 300.0f));
	Path.Pts[2] += CFPoint(FRANDRANGE(-300.0f, 300.0f), FRANDRANGE(-300.0f, 300.0f));
}


void Defcon::CGhostPart::Move(float fTime)
{
	// All we have to do is move along our predetermined spline 
	// and at the end, rebuild a ghost.

	CEnemy::Move(fTime);

	// We're moving along a spline, so determine our normalized distance along it.

	if(Age < MaxAge)
	{
		Path.CalcPt(powf(Age / MaxAge, 0.75f), Position);
		Position.x = gpArena->WrapX(Position.x);
		return;
	}

	this->MarkAsDead();
}


void Defcon::CGhostPart::Draw(FPainter& Painter, const I2DCoordMapper& mapper)
{
}


void Defcon::CGhostPart::Explode(CGameObjectCollection& Debris)
{
	// Ghost parts don't explode.
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
