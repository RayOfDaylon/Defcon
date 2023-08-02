// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	reformer.cpp
	Reformer enemy type for Defcon game.
*/


#include "reformer.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"



Defcon::CReformer::CReformer()
{
	ParentType = Type;
	Type       = EObjType::REFORMER;

	PointValue      = REFORMER_VALUE;
	VerticalOffset  = 0.0f;
	Frequency       = 2.0f;
	RadarColor      = MakeColorFromComponents(191, 33, 33);
	AnimSpeed       = FRAND * 0.35f + 0.65f;
	NumParts        = IRAND(4) + 4;
	SpinVelocity    = SFRAND;
	SpinAngle       = FRAND;
	MaxSpinVelocity = FRANDRANGE(1.0f, 5.0f);
	FiringCountdown = FRANDRANGE(2.0f, 4.0f);

	BboxRadius.Set(10, 10); // todo: why so explicit?
	Orientation.Fwd.Set(1.0f, 0.0f);
}


Defcon::CReformer::~CReformer()
{
}


#ifdef _DEBUG
const char* Defcon::CReformer::GetClassname() const
{
	static char* psz = "Reformer";
	return psz;
};
#endif


void Defcon::CReformer::Move(float DeltaTime)
{
	// Just float around drifting horizontally.

	CEnemy::Move(DeltaTime);
	Inertia = Position;

	Orientation.Fwd.y = 0.1f * (float)sin(Frequency * (VerticalOffset + Age)); 

	ConsiderFiringBullet(DeltaTime);

	SpinVelocity = (float)(sin(Age * PI * MaxSpinVelocity));
	SpinAngle += (SpinVelocity * DeltaTime);

	Position.MulAdd(Orientation.Fwd, DeltaTime * 50.0f);

	Inertia = Position - Inertia;
}


void Defcon::CReformer::ConsiderFiringBullet(float DeltaTime)
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


void Defcon::CReformer::Draw(FPaintArguments& PaintArgs, const I2DCoordMapper& Mapper)
{
	PartLocations[0] = Position;

	float F = (float)fmod(Age, AnimSpeed) / AnimSpeed;

	// Draw the parts in a circle around a central part.
	const int32 N = NumParts - 1;
	int32 I;

	for(I = 0; I < N; I++)
	{
		const float T = (float)(TWO_PI * I / N + (SpinAngle * TWO_PI));
		CFPoint P((float)cos(T), (float)sin(T));
		const float R = (float)(sin(F * PI) * 5 + 10);
		BboxRadius.Set(R, R);
		P *= R;
		P += Position;
		PartLocations[I + 1] = P;
	}

	for(I = 0; I < NumParts; I++)
	{
		CFPoint P;
		Mapper.To(PartLocations[I], P);
		DrawPart(PaintArgs, P);
	}
}


void Defcon::CReformer::DrawPart(FPaintArguments& PaintArgs, const CFPoint& Where)
{
	auto& Info = GameObjectResources.Get(EObjType::REFORMERPART);

	CFPoint P = Where;
	const float OurWidth = Info.Size.X;

	if(P.x >= -OurWidth && P.x <= PaintArgs.GetWidth() + OurWidth)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float F = (NumCels - 1) * PSIN(PI * fmod(Age, AnimSpeed) / AnimSpeed);

		const float Usize = 1.0f / NumCels;

		const auto S = Info.Size;
		const FSlateLayoutTransform Translation(FVector2D(P.x, P.y) - S / 2);
		const auto Geometry = PaintArgs.AllottedGeometry->MakeChild(S, Translation);

		const float F2 = Usize * ROUND(F);
		FBox2f UVRegion(FVector2f(F2, 0.0f), FVector2f(F2 + Usize, 1.0f));
		Info.Atlas->Atlas.AtlasBrush.SetUVRegion(UVRegion);

		FSlateDrawElement::MakeBox(
			*PaintArgs.OutDrawElements,
			PaintArgs.LayerId,
			Geometry.ToPaintGeometry(),
			&Info.Atlas->Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			C_WHITE * PaintArgs.RenderOpacity);
	}
}


void Defcon::CReformer::OnAboutToDie()
{
	// Release parts.

	for(int32 I = 0; I < NumParts; I++)
	{
		gpArena->CreateEnemy(EObjType::REFORMERPART, PartLocations[I], 0.0f, EObjectCreationFlags::EnemyPart);
	}
}


void Defcon::CReformer::Explode(CGameObjectCollection& Debris)
{
	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	const auto ColorBase = EColor::Gray;

	for(int32 i = 0; i < 20; i++)
	{
		auto Flak = new CFlak;

		Flak->ColorbaseYoung = ColorBase;
		Flak->ColorbaseOld   = ColorBase;
		Flak->bCold          = true;
		Flak->LargestSize    = 4;
		Flak->bFade          = true;
		Flak->Position       = Position;
		Flak->Orientation    = Orientation;

		CFPoint Direction;
		const double T = FRAND * TWO_PI;
		
		Direction.Set((float)cos(T), (float)sin(T));

		// Debris has at least the object's momentum.
		Flak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		Flak->Orientation.Fwd *= FRANDRANGE(20, 32);
		Flak->Orientation.Fwd.MulAdd(Direction, FRANDRANGE(110, 140));

		Debris.Add(Flak);
	}
}

// ----------------------------------------------------------------------------


Defcon::CReformerPart::CReformerPart()
{
	ParentType = Type;
	Type       = EObjType::REFORMERPART;

	PointValue            = REFORMERPART_VALUE;
	RadarColor            = C_RED;
	Frequency             = 2.0f;
	HorzFrequency         = FRANDRANGE(1.0f, 3.0f);
	bCanBeInjured         = true;
	bIsCollisionInjurious = true;
	VerticalOffset        = (FRAND * PI);
	TimeTargetWithinRange = 0.0f;
	MergeTime             = 0.0f;

	Orientation.Fwd.Set(1.0f, 0.0f);

	CreateSprite(Type);
	BboxRadius = GameObjectResources.Get(Type).Size / 2;
}


Defcon::CReformerPart::~CReformerPart()
{
}


#ifdef _DEBUG
const char* Defcon::CReformerPart::GetClassname() const
{
	static char* psz = "Reformer_part";
	return psz;
};
#endif


void Defcon::CReformerPart::Move(float DeltaTime)
{
	// Move in slightly perturbed sine wave pattern.

	CEnemy::Move(DeltaTime);

	Inertia = Position;

	// todo: if neighbouring parts are within visual range, 
	// then move towards them. If close enough and 
	// maximum number reached or other parts are too distant,
	// generate a reformer and delete the parts. 

	const bool bMerging = (Age >= 2.0f);


	IGameObject* ClosestObject = nullptr;
	CFPoint BestDirection;
	float BestDistance = 1.0e+10f;

	if(bMerging)
	{
		IGameObject* Object = nullptr;
		for(;;)
		{
			IGameObject* Neighbour = gpArena->FindEnemy(this->GetType(), Object);

			if(Neighbour == nullptr)
			{
				break;
			}

			Object = Neighbour;

			if(Neighbour == this)
			{
				continue;
			}

			CFPoint Direction;
			const float Distance = gpArena->ShortestDirection(Position, Neighbour->Position, Direction);

			if(Distance < BestDistance)
			{
				ClosestObject = Neighbour;
				BestDistance  = Distance;
				BestDirection = Direction;
			}
		}
	}


	if(ClosestObject == nullptr)
	{
		if(TargetPtr == nullptr)
		{
			TimeTargetWithinRange = 0.0f;
		}
		else
		{
			const bool bVis = IsOurPositionVisible();

			// Update target-within-range information.
			if(TimeTargetWithinRange > 0.0f)
			{
				// Target was in range; See if it left range.
				if(!bVis)
				{
					TimeTargetWithinRange = 0.0f;
				}
				else
				{
					TimeTargetWithinRange += DeltaTime;
				}
			}
			else
			{
				// Target was out of range; See if it entered range.
				if(bVis)
				{
					TimeTargetWithinRange = DeltaTime;

					//m_targetOffset.Set(
					//	LERP(-100, 100, FRAND), 
					//	LERP(50, 90, FRAND) * SGN(Position.y - TargetPtr->Position.y));
					//Frequency = LERP(6, 12, FRAND);
					//m_amp = LERP(.33f, .9f, FRAND);
				}
			}

			CFPoint dir;
			const float Distance = gpArena->ShortestDirection(Position, TargetPtr->Position, dir);

			if(TimeTargetWithinRange > 0.75f)
			{
				if(Distance > ScreenSize.x * .4f)
				{
					Orientation.Fwd = dir;
					Orientation.Fwd.y = 0;
					Orientation.Fwd.Normalize();
				}
			}

			if(TimeTargetWithinRange
				&& Age > 1.0f 
				&& FRAND <= 0.007f
				&& SGN(Orientation.Fwd.x) == SGN(dir.x))
			{
				(void) gpArena->FireBullet(*this, Position, 1, 1);
				gpAudio->OutputSound(EAudioTrack::Swarmer);
			}
		}

		Amplitude = LERP(0.33f, 1.0f, PSIN(VerticalOffset+Age)) * 0.5f * ScreenSize.y;
		HalfwayAltitude = (float)(sin((VerticalOffset+Age)*0.6f) * 50 + (0.5f * ScreenSize.y));

		CFPoint P;

		if(Age < 0.7f)
		{
			P.x = Position.x + .2f * Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * FRANDRANGE(0.25f, 0.3f);
		}
		else
		{
			P.x = Position.x + Orientation.Fwd.x * HorzFrequency * DeltaTime * ScreenSize.x * FRANDRANGE(0.25f, 0.3f);
		}

		P.y = (float)sin(Frequency * (VerticalOffset + Age)) * Amplitude + HalfwayAltitude;

		Position = P;

		if(Age < 0.7f)
		{
			Position.y = LERP(OriginalPos.y, P.y, Age / 0.7f);
		}
	}
	else // ClosestObject != nullptr
	{
		if(BestDistance > 15.0f) 
		{
			Position = Inertia + (BestDirection * FRANDRANGE(150, 200) * DeltaTime);
		}
		else
		{
			// If we have been at rest for longer than 
			// 2 seconds, then form a reformer.
			MergeTime += DeltaTime;

			if(MergeTime >= 2.0f)
			{
				MarkAsDead();
				ClosestObject->MarkAsDead();
				Position.Avg(ClosestObject->Position);

				gpArena->CreateEnemy(EObjType::REFORMER, Position, 0.0f, 
					(EObjectCreationFlags)((int32)EObjectCreationFlags::NotMissionTarget | (int32)EObjectCreationFlags::NoMaterialization));
			}
		}
	}

	Inertia = Position - Inertia;
}


void Defcon::CReformerPart::Explode(CGameObjectCollection& debris)
{
	const auto cby = EColor::Gray;

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	for(int32 i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->ColorbaseYoung = cby;
		pFlak->ColorbaseOld = cby;
		pFlak->bCold = true;
		pFlak->LargestSize = 4;
		pFlak->bFade = true;

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
