// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	munchies.cpp
	Phred, Big Red, and Munchie enemies for Defcon game.

	These guys look like squarish PacMen and mimic
	baiters in arrival and movement, except they don't fire;
	collision is their goal. 
*/

#include "munchies.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::IMunchie::IMunchie()
{
	ParentType = Type;
	Type       = EObjType::IMUNCHIE;

	Drag                    = PLAYER_DRAG * 0.15f;
	MaxThrust               = PLAYER_MAXTHRUST * 9.0f;
	bIsCollisionInjurious   = true;
	bPreferTargetUnderside  = BRAND;
	Brightness              = FRANDRANGE(0.4f, 0.5f);
	SquakTime               = 0.0f;
	WrapsAroundVertically   = BRAND;
}


void Defcon::IMunchie::Tick(float DeltaTime)
{
	CFPoint orgpos = Position;

	if(TargetPtr != nullptr)
	{
		const float xd = GArena->HorzDistance(Position, TargetPtr->Position);
		float d = Drag;
		float m = Mass;

		// Increase drag as baiter near player.
		float t = NORM_(xd, 0.0f, 500.0f);
		t = FMath::Min(1.0f, t);
		//Drag = LERP(Drag*30.0f, Drag, t);
		Mass = LERP(Mass*3.0f, Mass, t);

		CEnemy::Tick(DeltaTime);

		Drag = d;
		Mass = m;
	}

	orgpos -= Position;
	Sprite->FlipHorizontal = (orgpos.x > 0);

	if(WrapsAroundVertically)
	{
		Position.y = GArena->WrapY(Position.y);
	}

	// Thrust in a way that will take us towards the player. 
	// Only thrust vertically if we are not within range.

	if(TargetPtr == nullptr)
	{
		return;
	}

	
	CFPoint Delta;
	const float VertSgn = bPreferTargetUnderside ? -1.0f : 1.0f;

	const CFPoint TargetPos = TargetPtr->Position + CFPoint(
		cosf(Age) * 300.0f * SGN(TargetPtr->Orientation.Fwd.x),
		VertSgn * (float)fabs(sin(Age)) * 50.0f + VertSgn * 50.0f);

	const float HorzDistance = GArena->ShortestDirection(Position, TargetPos, Delta);
	bool bMoveTowards = (HorzDistance > 150 || SGN(Orientation.Fwd.x) != SGN(TargetPtr->Orientation.Fwd.x));

	if(bMoveTowards)
	{
		const int32 ctl = (Delta.x < 0)	
				? ENavControl::Back
				: ENavControl::Fwd;

		const int32 ctl2 = (ctl ==  ENavControl::Back)
				? ENavControl::Fwd
				: ENavControl::Back;

		SetNavControl(ctl,  true, NavControlDuration(ctl));
		SetNavControl(ctl2, false, 0);
	}
	else
	{
		// Parallel the player.
		SetNavControl(ENavControl::Back, false, 0);
		SetNavControl(ENavControl::Fwd,  false, 0);
	}

	// Vertical.
	bMoveTowards = (HorzDistance > 50/* && SGN(Orientation.Fwd.y) != SGN(TargetPtr->Orientation.Fwd.y)*/);

	if(bMoveTowards)
	{
		const int32 ctl  = (Delta.y < 0) ? ENavControl::Down : ENavControl::Up;
		const int32 ctl2 = (ctl == ENavControl::Down) ? ENavControl::Up : ENavControl::Down;

		SetNavControl(ctl,  true, NavControlDuration(ctl));
		SetNavControl(ctl2, false, 0);
	}
	else
	{
		SetNavControl(ENavControl::Down, false, 0);
		SetNavControl(ENavControl::Up, false, 0);
	}

	// Audio
	if(HorzDistance < 350 && FRAND < 0.02f && SquakTime == 0.0f)
	{
		SquakTime = 0.0001f;
		GMessageMediator.PlaySound(AudioTrack);
	}

	if(SquakTime != 0.0f)
	{
		SquakTime += DeltaTime;

		if(SquakTime > 1.0f)
		{
			SquakTime = 0.0f;
		}
	}
}

// ----------------------------------------------------------------------------------

Defcon::CPhred::CPhred()
{
	ParentType = Type;
	Type       = EObjType::PHRED;

	AudioTrack = EAudioTrack::Phred;
	PointValue = PHRED_VALUE;
	RadarColor = MakeColorFromComponents(192, 128, 192);
	Mass       = PLAYER_MASS * 20;

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius = SpriteInfo.Size / 2;

	MunchieSpawnCountdown = Daylon::FRandRange(MUNCHIE_SPAWN_COUNTDOWN);
}


void Defcon::CPhred::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Spawn a munchie but only if we're near the player.

	if(!IsOurPositionVisible())
	{
		return;
	}

	MunchieSpawnCountdown -= DeltaTime;

	if(MunchieSpawnCountdown > 0.0f)
	{
		return;
	}

	MunchieSpawnCountdown = Daylon::FRandRange(MUNCHIE_SPAWN_COUNTDOWN);

	GArena->SpawnGameObjectNow(EObjType::MUNCHIE, GetType(), Position, EObjectCreationFlags::CleanerEnemy);
}

// ----------------------------------------------------------------------------------

Defcon::CMunchie::CMunchie()
{
	ParentType = Type;
	Type       = EObjType::MUNCHIE;

	AudioTrack = EAudioTrack::Munchie;
	PointValue = MUNCHIE_VALUE;
	RadarColor = MakeColorFromComponents(0, 192, 0);
	Mass       = PLAYER_MASS * 18;

	CreateSprite(Type);
	BboxRadius = GGameObjectResources.Get(Type).Size / 2;

	static const EColor ColorCodes[] =
	{
		EColor::Green,
		EColor::Orange,
		EColor::Red
	};

	ColorCode = ColorCodes[IRAND(array_size(ColorCodes))];

	Sprite->SetTint(GGameColors.GetColor(ColorCode, 1.0f));
}

// ----------------------------------------------------------------------------------

Defcon::CBigRed::CBigRed()
{
	ParentType = Type;
	Type       = EObjType::BIGRED;

	AudioTrack = EAudioTrack::BigRed;
	PointValue = BIGRED_VALUE;
	RadarColor = MakeColorFromComponents(255, 128, 0);
	Mass       = PLAYER_MASS * 22;

	CreateSprite(Type);
	BboxRadius = GGameObjectResources.Get(Type).Size / 2;
}
