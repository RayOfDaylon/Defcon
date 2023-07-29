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
	m_screensize.set((float)w, (float)h);
	m_planetCircumference = (float)circum;
	m_offset.set(0, 0);
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

	if(m_offset.x >= m_planetCircumference - m_screensize.x)
	{
		// The screen has the arena origin.
		float x = in.x;

		if(x < m_screensize.x * 2)
			x += m_planetCircumference;

		out.x = x - m_offset.x;
	}
	else
	{
		// The screen is not showing the arena origin.
		out.x = in.x - m_offset.x;

		if(m_offset.x > m_planetCircumference / 2 && in.x < m_screensize.x)
		{
			out.x = in.x + m_planetCircumference - m_offset.x;
		}
		else if(m_offset.x < m_screensize.x && out.x > m_planetCircumference / 2)
		{
			// rcg, july 3/04: we get arena coords that map 
			// to large screen coords instead of negative screen coords.
			out.x = in.x - m_planetCircumference - m_offset.x;
		}
	}
	
	out.y = m_screensize.y - in.y;
}


void Defcon::CArenaCoordMapper::From(const CFPoint& in, CFPoint& out) const
{
	// Map a point from screen space to arena space.
	out.x = in.x + m_offset.x;

	if(out.x < 0)
		out.x += m_planetCircumference;
	else if(out.x >= m_planetCircumference)
		out.x -= m_planetCircumference;

	out.y = m_screensize.y - in.y;
}


void Defcon::CArenaCoordMapper::SlideBy(float f)
{
	m_offset.x += f;
	if(m_offset.x < 0)
		 m_offset.x += m_planetCircumference;
	else if(m_offset.x >= m_planetCircumference)
		m_offset.x -= m_planetCircumference;
}

// ------------------------------------------------------------------------------------

void Defcon::CRadarCoordMapper::Init(int w, int h, int circum)
{
	m_screensize.set((float)w, (float)h);
	m_planetCircumference = (float)circum;
	//m_offset.set(0, 0);

	m_scale = m_radarSize;
	m_scale /= CFPoint((float)m_planetCircumference, (float)h);
}


void Defcon::CRadarCoordMapper::To(const CFPoint& in, CFPoint& out) const
{
	// Map from Cartesian pixel space to arena radar space.
	// The ship is always in the horz center of the radar.

	//CPlayer* pRadar = m_pPlayer;
	check(m_pPlayer);

	//if(pRadar != nullptr)
	{
		float x = m_pPlayer->m_pos.x;
		x -= m_planetCircumference / 2;

		if(x < 0)
			x += m_planetCircumference;

		m_offset.x = x;
	}
	// m_offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	if(in.x < m_offset.x)
		out.x = in.x + m_planetCircumference - m_offset.x;
	else
		out.x = in.x - m_offset.x;

	out.y = in.y;
	out *= m_scale;
	
	out.y = m_radarSize.y - out.y;
}


void Defcon::CRadarCoordMapper::From(const CFPoint& in, CFPoint& out) const
{
	// Map a point from radar space to planet space.


	//CPlayer* pRadar = m_pPlayer;

	check(m_pPlayer);

	//if(pRadar != nullptr)
	{
		float x = m_pPlayer->m_pos.x;
		x -= m_planetCircumference / 2;
		if(x < 0)
			x += m_planetCircumference;
		m_offset.x = x;
	}

	// m_offset.x now has the point of arena space that 
	// starts on the left edge of the radar.
	out.x = in.x / m_scale.x + m_offset.x;

	if(out.x < 0)
		out.x += m_planetCircumference;
	else if(out.x >= m_planetCircumference)
		out.x -= m_planetCircumference;

	out.y = (m_radarSize.y - in.y) / m_scale.y;


#if 0	
	out.x = in.x + m_offset.x;

	if(out.x < 0)
		out.x += m_planetCircumference;
	else if(out.x >= m_planetCircumference)
		out.x -= m_planetCircumference;

	out.y = m_screensize.y - in.y;
#endif
}


void Defcon::CRadarCoordMapper::SlideBy(float f)
{
	m_offset.x += f;

	if(m_offset.x < 0)
		 m_offset.x += m_planetCircumference;
	else if(m_offset.x >= m_planetCircumference)
		m_offset.x -= m_planetCircumference;
}

