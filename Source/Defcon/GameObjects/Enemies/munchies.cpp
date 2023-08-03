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
}


void Defcon::IMunchie::Move(float DeltaTime)
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

		CEnemy::Move(DeltaTime);

		Drag = d;
		Mass = m;
	}

	orgpos -= Position;
	Sprite->FlipHorizontal = (orgpos.x > 0);


	// Thrust in a way that will take us towards the player. 
	// Only thrust vertically if we are not within range.

	if(TargetPtr == nullptr)
	{
		return;
	}

	
	CFPoint Delta;
	const float VertSgn = bPreferTargetUnderside ? -1.0f : 1.0f;

	const CFPoint TargetPos = TargetPtr->Position + CFPoint(
		(float)cos(Age) * 300.0f * SGN(TargetPtr->Orientation.Fwd.x),
		VertSgn * (float)fabs(sin(Age)) * 50.0f + VertSgn * 50.0f);

	const float HorzDistance = GArena->ShortestDirection(Position, TargetPos, Delta);
	bool bMoveTowards = (HorzDistance > 150 || SGN(Orientation.Fwd.x) != SGN(TargetPtr->Orientation.Fwd.x));

	if(bMoveTowards)
	{
		int32 ctl = (Delta.x < 0)	
				? ILiveGameObject::ctlBack
				: ILiveGameObject::ctlFwd;

		int32 ctl2 = (ctl ==  ILiveGameObject::ctlBack)
				? ILiveGameObject::ctlFwd
				: ILiveGameObject::ctlBack;

		if(NavControlDuration(ctl) == 0)
		{
			ControlStartTime[ctl] = GameTime();
		}
		SetNavControl(ctl,  true,  ControlStartTime[ctl]);
		SetNavControl(ctl2, false, 0);
	}
	else
	{
		// Parallel the player.
		SetNavControl(ILiveGameObject::ctlBack, false, 0);
		SetNavControl(ILiveGameObject::ctlFwd,  false, 0);
	}

	// Vertical.
	bMoveTowards = (HorzDistance > 50/* && SGN(Orientation.Fwd.y) != SGN(TargetPtr->Orientation.Fwd.y)*/);

	if(bMoveTowards)
	{
		const int32 ctl  = (Delta.y < 0) ? ILiveGameObject::ctlDown : ILiveGameObject::ctlUp;
		const int32 ctl2 = (ctl == ILiveGameObject::ctlDown) ? ILiveGameObject::ctlUp : ILiveGameObject::ctlDown;

		if(NavControlDuration(ctl) == 0)
		{
			ControlStartTime[ctl] = GameTime();
		}
		SetNavControl(ctl,  true,  ControlStartTime[ctl]);
		SetNavControl(ctl2, false, 0);
	}
	else
	{
		SetNavControl(ILiveGameObject::ctlDown, false, 0);
		SetNavControl(ILiveGameObject::ctlUp, false, 0);
	}

	// Audio
	if(HorzDistance < 350 && FRAND < 0.02f && SquakTime == 0.0f)
	{
		SquakTime = 0.0001f;
		gpAudio->OutputSound(AudioTrack);
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
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius = SpriteInfo.Size / 2;
}

// ----------------------------------------------------------------------------------

Defcon::CMunchie::CMunchie()
{
	ParentType = Type;
	Type = EObjType::MUNCHIE;

	AudioTrack = EAudioTrack::Munchie;
	PointValue = MUNCHIE_VALUE;
	RadarColor = MakeColorFromComponents(0, 192, 0);
	Mass = PLAYER_MASS * 18;

	CreateSprite(Type);
	BboxRadius = GameObjectResources.Get(Type).Size / 2;
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
	BboxRadius = GameObjectResources.Get(Type).Size / 2;
}
