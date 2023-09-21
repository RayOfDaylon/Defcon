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

	for(int32 I = 1; I < NumParts; I++)
	{
		const float Low = FRANDRANGE(0.5f, 1.0f);
		PartRadii[I].Set(Low, FRANDRANGE(Low, 1.5f)); 

		PartRadiiSpeed[I] = FRANDRANGE(1.0f, 2.0f);
	}

	const auto& Info = GGameObjectResources.Get(EObjType::GHOSTPART);
	BboxRadius = Info.Size * 0.5f; // seems too small, yet... here we are
}


Defcon::CGhost::~CGhost()
{
}


void Defcon::CGhost::ConsiderFiringBullet(float DeltaTime)
{
	if(!GArena->IsPointVisible(Position) || TargetPtr == nullptr)
	{
		return;
	}
		
	// Hold fire if target is below ground
	if(TargetPtr->Position.y < GArena->GetTerrainElev(TargetPtr->Position.x))
	{
		return;
	}

	FiringCountdown -= DeltaTime;

	if(FiringCountdown <= 0.0f)
	{
		(void) GArena->FireBullet(*this, Position, 1, 1);

		// The time to fire goes down as the player XP increases.

		const float XP = (float)GGameMatch->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		FiringCountdown = LERP(3.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 1.0f);
	}
}


void Defcon::CGhost::Tick(float DeltaTime)
{
	// Just float around drifting horizontally.

	if(DispersalCountdown > 0.0f)
	{
		DispersalCountdown -= DeltaTime;
		return;
	}

	// We are in normal form (not dispersed).

	CEnemy::Tick(DeltaTime);

	Inertia = Position;

	Orientation.Fwd.y = 0.1f * sinf(Frequency * (OffsetY + Age)); 

	ConsiderFiringBullet(DeltaTime);

	//SpinVelocity = 1.0f;//sin(Age * PI * m_fSpinVelMax);
	SpinAngle += (SpinVelocity * DeltaTime);

	Position.MulAdd(Orientation.Fwd, DeltaTime * 50.0f);
	Inertia = Position - Inertia;

	// See if we need to disperse (player ship got too close).

	if(TargetPtr != nullptr && !MarkedForDeath())
	{
		CFPoint Direction;
		const float Distance = GArena->ShortestDirection(Position, TargetPtr->Position, Direction);

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
			NewPos.x = Position.x + SFRAND * GArena->GetDisplayWidth() / 1.5f;
			NewPos.y = FRANDRANGE(0.125f, 0.875f) * GArena->GetHeight();

			// Don't modulate newloc; it will cause wrong path animation if path cross x-origin.
			// Modulation must happen in ghostpart::move.

			for(int32 i = 1; i < NumParts; i++)
			{
				auto GhostPart = (CGhostPart*)GArena->SpawnGameObjectNow(EObjType::GHOSTPART, GetType(), PartLocs[i], EObjectCreationFlags::EnemyPart);

				GhostPart->SetCollisionInjurious(false);
				GhostPart->SetFlightDuration(FlightTime);
				GhostPart->SetFlightPath(Position, NewPos);
			}

			// Now move ourselves. Do this last because we use our original Position as the 
			// source position of the dispersal in the above loop.

			Position   = NewPos;
			Position.x = GArena->WrapX(Position.x);
		
			GAudio->OutputSound(EAudioTrack::Ghostflight);
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
		CFPoint Pt2(cosf(T), sinf(T));
		const float Radius = (float)(sin((F /*+ FRAND * 3*/) * PI) * R * Daylon::Lerp(PartRadii[I + 1], PSIN(Age * PartRadiiSpeed[I + 1])) /*+ 5.0f*/);

		Pt2 *= Radius;
		Pt2 += Position;
		PartLocs[I + 1] = Pt2;
	}

	for(int32 I = 0; I < NumParts; I++)
	{
		CFPoint pt;
		mapper.To(PartLocs[I], pt);
		DrawPart(Painter, pt);
	}
}


void Defcon::CGhost::DrawPart(FPainter& Painter, const CFPoint& Pt)
{
	auto& Info = GGameObjectResources.Get(EObjType::GHOSTPART);

	const int32 W = Info.Size.X;

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
	int32 N = (int32)(FRAND * 30 + 30);
	float MaxSize = FRAND * 5 + 3;

/*
	// Don'T use huge particles ever; it just looks silly in the end.
	if(FRAND < .08)
		MaxSize = 14;
*/

	// Define which color to make the Debris.
	auto ColorBase = GetExplosionColorBase();
	MaxSize *= GetExplosionMass();
	
	if(IRAND(3) == 1)
	{
		ColorBase = EColor::Gray;
	}

	bool bDieOff = (FRAND >= 0.25f);
	int32 I;


	//float BrightBase;
	//IGameObject* pFireblast = CreateFireblast(Debris, BrightBase);

	for(I = 0; I < N; I++)
	{
		CGlowingFlak* FlakPtr = new CGlowingFlak;

		FlakPtr->ColorbaseYoung = ColorBase;
		FlakPtr->LargestSize = MaxSize;
		FlakPtr->bFade = bDieOff;

		FlakPtr->Position = Position;
		FlakPtr->Orientation = Orientation;

		CFPoint Direction;
		Direction.SetRandomVector();

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
			ColorBase = GetExplosionColorBase();
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
			Direction.SetRandomVector();

			FlakPtr->Orientation.Fwd = Inertia;
			FlakPtr->Orientation.Fwd *= FRANDRANGE(30, 42) * 1.5f;

			const float Speed = FRANDRANGE(22, 67);

			FlakPtr->Orientation.Fwd.MulAdd(Direction, Speed);

			Debris.Add(FlakPtr);
		}
	}


	FExplosionParams Params;

	Params.NumParticles    = 20;
	Params.bFade           = bDieOff;
	Params.bCold           = true;
	Params.MaxParticleSize = 8;
	Params.InertiaScale    = 1.5f;
	Params.YoungColor[0]   = 
	Params.YoungColor[1]   = 
	Params.OldColor[0]     = 
	Params.OldColor[1]     = EColor::Gray;
	Params.MinSpeed        = 110;
	Params.MaxSpeed        = 140;

	AddExplosionDebris(Params, Debris);
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
	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


void Defcon::CGhostPart::OnFinishedCreating(const FMetadata& Options)
{
	Super::OnFinishedCreating(Options);

	SetCollisionInjurious(false);
}


Defcon::CGhostPart::~CGhostPart()
{
}


void Defcon::CGhostPart::SetFlightPath(const CFPoint& From, const CFPoint& To)
{
	// from and to must be unmodulated.

	Path.Pts[0] = Path.Pts[1] = From;
	Path.Pts[2] = Path.Pts[3] = To;

	// Make the control points match their anchor but with a random offset.
	Path.Pts[1] += CFPoint(FRANDRANGE(-300.0f, 300.0f), FRANDRANGE(-300.0f, 300.0f));
	Path.Pts[2] += CFPoint(FRANDRANGE(-300.0f, 300.0f), FRANDRANGE(-300.0f, 300.0f));
}


void Defcon::CGhostPart::Tick(float DeltaTime)
{
	// All we have to do is move along our predetermined spline 
	// and at the end, rebuild a ghost.

	CEnemy::Tick(DeltaTime);

	// We're moving along a spline, so determine our normalized distance along it.

	if(Age < MaxAge)
	{
		Path.CalcPt(powf(Age / MaxAge, 0.75f), Position);
		Position.x = GArena->WrapX(Position.x);
		return;
	}

	MarkAsDead();
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
