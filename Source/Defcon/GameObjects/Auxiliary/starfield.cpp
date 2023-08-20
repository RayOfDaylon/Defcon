// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


/*
	starfield.cpp
	Simple animated 3D starfield.

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
	Age = 0;
	bCanBeInjured = false;
	bIsCollisionInjurious = false;

	m_count = array_size(m_stars);
}


void Defcon::CStarfield::InitStarfield(int32 w, int32 h, float fRange, bool bMorphColor, FLinearColor nearColor)
{
	// Initialize a starfield to pixel dimensions w x h.
	m_w = w;
	m_h = h;
	m_bMorphColor = bMorphColor;

	// Start by putting the stars mostly in the back.
	// As the update process brings them nearer, 
	// they will cycle to the background again.
		
	const int32 n = array_size(m_stars);
	const FLinearColor colors[] = 
		{
			C_WHITE, C_WHITE, C_WHITE, C_WHITE,
			C_WHITE, C_WHITE, C_WHITE, C_WHITE,
			C_LIGHTERBLUE, C_LIGHTYELLOW, C_LIGHTYELLOW, C_YELLOW,
			C_ORANGE, C_CYAN 
		};

	for(int32 i = 0; i < n; i++)
	{
		m_stars[i].Position.Set(SFRAND * 4000, SFRAND * 4000, kBackplane - FRAND * fRange);
		m_stars[i].m_colorFar = colors[IRAND(array_size(colors))];
		
		if(m_bMorphColor)
			m_stars[i].m_colorNear = nearColor;
	}
}


void Defcon::CStarfield::Move(float DeltaTime)
{
	Age += f;

	// Bring the stars closer.
	const int32 n = array_size(m_stars);

	for(int32 i = 0; i < n; i++)
	{
		m_stars[i].Position.z -= (f * kSpeed * m_fSpeedMul);
		// If too close, wrap it back.
		if(m_stars[i].Position.z < kForeplane)
			m_stars[i].Position.Set(SFRAND * 2000, SFRAND * 2000, kBackplane - FRAND*7);
	}

	m_fAngle += (f * kSpeedRot * m_fSpeedRotMul);
	m_fAngle = (float)fmod(m_fAngle, 360.0);
}


void Defcon::CStarfield::Draw(FPaintArguments& Painter, const I2DCoordMapper& mapper)
{
	// The location of the field has Position as our center,
	// so Position - w/2, h/2 to Position + (w/2,h/2) is the 
	// part of the framebuf we draw on.

	CFPoint pt;
	mapper.To(Position, pt);

	const int32 n = GetNumStars();

	const CFPoint halfsize(0.5f * m_w, 0.5f * m_h);
	CFRect rect(pt);
	rect.Inflate(halfsize);


	CFPoint scale(1.0f, (float)m_h/m_w);
	CFPoint offset = pt;//rect.UR;
	//offset += CFPoint(-0.5f* m_w, -0.5f * m_h);
	CFPoint inc(1,1);

	for(int32 i = 0; i < n; i++)
	{
		// Project 3D star location to 2D.
		CFPoint starproj;

		starproj.Set(m_stars[i].Position.x / m_stars[i].Position.z,
					m_stars[i].Position.y / m_stars[i].Position.z);

		starproj.mul(scale);
		starproj.rotate(m_fAngle);
		starproj += offset;

		float c;

		if(rect.PtInside(starproj))
		{
			CFRect r(starproj);
			r.Inflate(inc);
			c = 0.25f;//C_DARKER;


			if(m_stars[i].Position.z < 4.0f)
			{
				r.UR += inc;
				c = BRAND ? 0.5f/*C_DARK*/ : 0.75f/*C_LIGHT*/;
			}

			if(m_stars[i].Position.z < 2.0f)
			{
				r.UR += inc;
				c = BRAND ? 0.875f/*C_BRIGHT*/ : 1.0f/*C_WHITE*/;
			}

			if(m_bFat && BRAND)
				r.UR += inc;
			
#if 0
			int32 x1, y1, x2, y2;
			r.order();
			r.Classicize(x1, y1, x2, y2);
#endif

			CMaskMap& mask = gBitmaps.GetMask(ROUND(r.LL.Distance(r.UR)), 0);
			FLinearColor cf = MakeBlendedColor(C_BLACK, m_stars[i].m_colorFar, c);

			if(m_bMorphColor && m_stars[i].Position.z <= 3.0f)
				cf = MakeBlendedColor(m_stars[i].m_colorNear, cf,
					m_stars[i].Position.z / 3.0f);

			mask.ColorWith(cf, Painter, ROUND(starproj.x), ROUND(starproj.y));
		}
	}
}


#endif
