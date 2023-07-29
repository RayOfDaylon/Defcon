// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


/*
	starfield.cpp
	Simple animated 3D starfield.
	Copyright 2004-2021 Daylon Graphics Ltd.

	rcg		jun 24/04	Created.
*/


#include "starfield.h"


#include "Common/util_core.h"
#include "Common/util_color.h"


#include "GameObjects/obj_types.h"
#include "Main/mapper.h"

#if 0

constexpr float kBackplane	= 10.0f;
constexpr float kForeplane	= 0.1f;
constexpr float kPlanedist	= kBackplane - kForeplane;
constexpr float kSpeed		= 0.25f;
constexpr float kSpeedRot	= 3.0f;


Defcon::CStarfield::CStarfield()
{
	m_fAge = 0;
	m_bCanBeInjured = false;
	m_bIsCollisionInjurious = false;

	m_count = array_size(m_stars);
}


void Defcon::CStarfield::InitStarfield(size_t w, size_t h, float fRange, bool bMorphColor, FLinearColor nearColor)
{
	// Initialize a starfield to pixel dimensions w x h.
	m_w = w;
	m_h = h;
	m_bMorphColor = bMorphColor;

	// Start by putting the stars mostly in the back.
	// As the update process brings them nearer, 
	// they will cycle to the background again.
		
	const size_t n = array_size(m_stars);
	const FLinearColor colors[] = 
		{
			C_WHITE, C_WHITE, C_WHITE, C_WHITE,
			C_WHITE, C_WHITE, C_WHITE, C_WHITE,
			C_LIGHTERBLUE, C_LIGHTYELLOW, C_LIGHTYELLOW, C_YELLOW,
			C_ORANGE, C_CYAN 
		};

	for(size_t i = 0; i < n; i++)
	{
		m_stars[i].m_pos.set(SFRAND * 4000, SFRAND * 4000, kBackplane - FRAND * fRange);
		m_stars[i].m_colorFar = colors[IRAND(array_size(colors))];
		
		if(m_bMorphColor)
			m_stars[i].m_colorNear = nearColor;
	}
}


void Defcon::CStarfield::Move(float f)
{
	m_fAge += f;

	// Bring the stars closer.
	const size_t n = array_size(m_stars);

	for(size_t i = 0; i < n; i++)
	{
		m_stars[i].m_pos.z -= (f * kSpeed * m_fSpeedMul);
		// If too close, wrap it back.
		if(m_stars[i].m_pos.z < kForeplane)
			m_stars[i].m_pos.set(SFRAND * 2000, SFRAND * 2000, kBackplane - FRAND*7);
	}

	m_fAngle += (f * kSpeedRot * m_fSpeedRotMul);
	m_fAngle = (float)fmod(m_fAngle, 360.0);
}


void Defcon::CStarfield::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	// The location of the field has m_pos as our center,
	// so m_pos - w/2, h/2 to m_pos + (w/2,h/2) is the 
	// part of the framebuf we draw on.

	CFPoint pt;
	mapper.To(m_pos, pt);

	const size_t n = this->GetNumStars();

	const CFPoint halfsize(0.5f * m_w, 0.5f * m_h);
	CFRect rect(pt);
	rect.inflate(halfsize);


	CFPoint scale(1.0f, (float)m_h/m_w);
	CFPoint offset = pt;//rect.UR;
	//offset += CFPoint(-0.5f* m_w, -0.5f * m_h);
	CFPoint inc(1,1);

	for(size_t i = 0; i < n; i++)
	{
		// Project 3D star location to 2D.
		CFPoint starproj;

		starproj.set(m_stars[i].m_pos.x / m_stars[i].m_pos.z,
					m_stars[i].m_pos.y / m_stars[i].m_pos.z);

		starproj.mul(scale);
		starproj.rotate(m_fAngle);
		starproj += offset;

		float c;

		if(rect.ptinside(starproj))
		{
			CFRect r(starproj);
			r.inflate(inc);
			c = 0.25f;//C_DARKER;


			if(m_stars[i].m_pos.z < 4.0f)
			{
				r.UR += inc;
				c = BRAND ? 0.5f/*C_DARK*/ : 0.75f/*C_LIGHT*/;
			}

			if(m_stars[i].m_pos.z < 2.0f)
			{
				r.UR += inc;
				c = BRAND ? 0.875f/*C_BRIGHT*/ : 1.0f/*C_WHITE*/;
			}

			if(m_bFat && BRAND)
				r.UR += inc;
			
#if 0
			int x1, y1, x2, y2;
			r.order();
			r.classicize(x1, y1, x2, y2);
#endif

			CMaskMap& mask = gBitmaps.GetMask(ROUND(r.LL.distance(r.UR)), 0);
			FLinearColor cf = MakeBlendedColor(C_BLACK, m_stars[i].m_colorFar, c);

			if(m_bMorphColor && m_stars[i].m_pos.z <= 3.0f)
				cf = MakeBlendedColor(m_stars[i].m_colorNear, cf,
					m_stars[i].m_pos.z / 3.0f);

			mask.ColorWith(cf, framebuf, ROUND(starproj.x), ROUND(starproj.y));
		}
	}
}


#endif