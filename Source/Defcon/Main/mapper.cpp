// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mapper.cpp
	Coordinate mapping support for Defcon game.
*/


#include "mapper.h"
#include "Common/util_core.h"
#include "GameObjects/gameobj.h"
#include "Globals/prefs.h"

#include "Arenas/DefconPlayViewBase.h"



// -------------------------------------------------------

void Defcon::CArenaCoordMapper::Init(float w, float h, float circum)
{
	ScreenSize.Set(w, h);
	PlanetCircumference = circum;
	HalfCircumference   = circum * 0.5f;
	Offset.Set(0, 0);
}


void Defcon::CArenaCoordMapper::To(const CFPoint& In, CFPoint& Result) const
{
	// Map from Cartesian arena space to screen space.
	/*if(pt >= t || pt > t - sw)
		pt' = (pt - t)
	else
		pt' = (pt + n - t)

	pt' *= s;*/

	// offset is the position of the screen relative to 
	// the arena. If offset is (1,1), for example, then 
	// the arena's origin maps to the screen's cartesian (1,1).

	if(Offset.x >= PlanetCircumference - ScreenSize.x)
	{
		// The screen has the arena origin.
		float x = In.x;

		if(x < ScreenSize.x * 2)
		{
			x += PlanetCircumference;
		}

		Result.x = x - Offset.x;
	}
	else
	{
		// The screen is not showing the arena origin.
		Result.x = In.x - Offset.x;

		if(Offset.x > HalfCircumference && In.x < ScreenSize.x)
		{
			Result.x = In.x + PlanetCircumference - Offset.x;
		}
		else if(Offset.x < ScreenSize.x && Result.x > HalfCircumference)
		{
			// rcg, july 3/04: we get arena coords that map 
			// to large screen coords instead of negative screen coords.
			Result.x = In.x - PlanetCircumference - Offset.x;
		}
	}
	
	Result.y = ScreenSize.y - In.y;
}


void Defcon::CArenaCoordMapper::From(const CFPoint& In, CFPoint& Result) const
{
	// Map a point from screen space to arena space.
	Result.x = In.x + Offset.x;

	if(Result.x < 0)
	{
		Result.x += PlanetCircumference;
	}
	else if(Result.x >= PlanetCircumference)
	{
		Result.x -= PlanetCircumference;
	}

	Result.y = ScreenSize.y - In.y;
}


void Defcon::CArenaCoordMapper::SlideBy(float XAmount)
{
	Offset.x += XAmount;

	if(Offset.x < 0)
	{
		 Offset.x += PlanetCircumference;
	}
	else if(Offset.x >= PlanetCircumference)
	{
		Offset.x -= PlanetCircumference;
	}
}

// ------------------------------------------------------------------------------------

void Defcon::CRadarCoordMapper::Init(float w, float h, float circum)
{
	ScreenSize.Set(w, h);
	PlanetCircumference = circum;
	HalfCircumference = circum * 0.5f;

	Scale = RadarSize;
	Scale /= CFPoint(PlanetCircumference, h);
}


void Defcon::CRadarCoordMapper::To(const CFPoint& In, CFPoint& Result) const
{
	// Map from Cartesian pixel space to arena radar space.
	// The ship is always in the horz center of the radar.

	// todo: we could save time by subclassing player-centric and non-player-centric versions of the radar mapper
	// since the mapper type is constant for the game lifetime.

	if(RADAR_IS_PLAYER_CENTRIC)
	{
		check(Player != nullptr);

		float x = Player->Position.x - HalfCircumference;

		if(x < 0)
		{
			x += PlanetCircumference;
		}

		Offset.x = x;
	}

	// Offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	if(In.x < Offset.x)
	{
		Result.x = In.x + PlanetCircumference - Offset.x;
	}
	else
	{
		Result.x = In.x - Offset.x;
	}

	Result.y = In.y;
	Result *= Scale;
	
	Result.y = RadarSize.y - Result.y;
}


void Defcon::CRadarCoordMapper::From(const CFPoint& In, CFPoint& Result) const
{
	// Map a point from radar space to planet space.
	// This probably isn't used, but still.
	// todo: if we're not player centric, this code is wrong.

	check(Player);

	float x = Player->Position.x;
	x -= HalfCircumference;

	if(x < 0)
	{
		x += PlanetCircumference;
	}

	Offset.x = x;

	// Offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	Result.x = In.x / Scale.x + Offset.x;

	if(Result.x < 0)
	{
		Result.x += PlanetCircumference;
	}
	else if(Result.x >= PlanetCircumference)
	{
		Result.x -= PlanetCircumference;
	}

	Result.y = (RadarSize.y - In.y) / Scale.y;
}


void Defcon::CRadarCoordMapper::SlideBy(float XAmount)
{
	Offset.x += XAmount;

	if(Offset.x < 0)
	{
		 Offset.x += PlanetCircumference;
	}
	else if(Offset.x >= PlanetCircumference)
	{
		Offset.x -= PlanetCircumference;
	}
}


#if 0
	// Test mapper.
	{
		CFPoint in, out, final;
		for(in.x = -50; in.x < 50; in.x++)
		{
			MapperPtr->To(in, out);
			MapperPtr->From(out, final);
			int32 inx = ROUND(in.x);
			int32 fx  = ROUND(final.x);
			if(inx != fx)
			{
				char sz[100];
				UE_LOG(LogGame, Log, TEXT("Mapping x = %.3f to %.3f back to %.3f"), in.x, out.x, final.x);
				OutputDebugString(sz);
			}
		}
	}
#endif
