// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mapper.cpp
	Coordinate mapping support for Defcon game.
*/


#include "mapper.h"
#include "Common/util_core.h"
#include "GameObjects/gameobj.h"

#include "Arenas/DefconPlayViewBase.h"



// -------------------------------------------------------

void Defcon::CArenaCoordMapper::Init(int w, int h, int circum)
{
	ScreenSize.Set((float)w, (float)h);
	PlanetCircumference = (float)circum;
	Offset.Set(0, 0);
}


void Defcon::CArenaCoordMapper::To(const CFPoint& in, CFPoint& out) const
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
		float x = in.x;

		if(x < ScreenSize.x * 2)
			x += PlanetCircumference;

		out.x = x - Offset.x;
	}
	else
	{
		// The screen is not showing the arena origin.
		out.x = in.x - Offset.x;

		if(Offset.x > PlanetCircumference / 2 && in.x < ScreenSize.x)
		{
			out.x = in.x + PlanetCircumference - Offset.x;
		}
		else if(Offset.x < ScreenSize.x && out.x > PlanetCircumference / 2)
		{
			// rcg, july 3/04: we get arena coords that map 
			// to large screen coords instead of negative screen coords.
			out.x = in.x - PlanetCircumference - Offset.x;
		}
	}
	
	out.y = ScreenSize.y - in.y;
}


void Defcon::CArenaCoordMapper::From(const CFPoint& in, CFPoint& out) const
{
	// Map a point from screen space to arena space.
	out.x = in.x + Offset.x;

	if(out.x < 0)
		out.x += PlanetCircumference;
	else if(out.x >= PlanetCircumference)
		out.x -= PlanetCircumference;

	out.y = ScreenSize.y - in.y;
}


void Defcon::CArenaCoordMapper::SlideBy(float f)
{
	Offset.x += f;
	if(Offset.x < 0)
		 Offset.x += PlanetCircumference;
	else if(Offset.x >= PlanetCircumference)
		Offset.x -= PlanetCircumference;
}

// ------------------------------------------------------------------------------------

void Defcon::CRadarCoordMapper::Init(int w, int h, int circum)
{
	ScreenSize.Set((float)w, (float)h);
	PlanetCircumference = (float)circum;
	//Offset.Set(0, 0);

	Scale = m_radarSize;
	Scale /= CFPoint((float)PlanetCircumference, (float)h);
}


void Defcon::CRadarCoordMapper::To(const CFPoint& in, CFPoint& out) const
{
	// Map from Cartesian pixel space to arena radar space.
	// The ship is always in the horz center of the radar.

	//CPlayer* pRadar = m_pPlayer;
	check(m_pPlayer);

	//if(pRadar != nullptr)
	{
		float x = m_pPlayer->Position.x;
		x -= PlanetCircumference / 2;

		if(x < 0)
			x += PlanetCircumference;

		Offset.x = x;
	}
	// Offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	if(in.x < Offset.x)
		out.x = in.x + PlanetCircumference - Offset.x;
	else
		out.x = in.x - Offset.x;

	out.y = in.y;
	out *= Scale;
	
	out.y = m_radarSize.y - out.y;
}


void Defcon::CRadarCoordMapper::From(const CFPoint& in, CFPoint& out) const
{
	// Map a point from radar space to planet space.


	//CPlayer* pRadar = m_pPlayer;

	check(m_pPlayer);

	//if(pRadar != nullptr)
	{
		float x = m_pPlayer->Position.x;
		x -= PlanetCircumference / 2;
		if(x < 0)
			x += PlanetCircumference;
		Offset.x = x;
	}

	// Offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	out.x = in.x / Scale.x + Offset.x;

	if(out.x < 0)
		out.x += PlanetCircumference;
	else if(out.x >= PlanetCircumference)
		out.x -= PlanetCircumference;

	out.y = (m_radarSize.y - in.y) / Scale.y;


#if 0	
	out.x = in.x + Offset.x;

	if(out.x < 0)
		out.x += PlanetCircumference;
	else if(out.x >= PlanetCircumference)
		out.x -= PlanetCircumference;

	out.y = ScreenSize.y - in.y;
#endif
}


void Defcon::CRadarCoordMapper::SlideBy(float f)
{
	Offset.x += f;

	if(Offset.x < 0)
		 Offset.x += PlanetCircumference;
	else if(Offset.x >= PlanetCircumference)
		Offset.x -= PlanetCircumference;
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
