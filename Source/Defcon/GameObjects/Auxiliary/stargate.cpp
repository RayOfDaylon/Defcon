// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	stargate.cpp
	Stargate object for Defcon game, which implements the hyperspatial jump portal.
*/


#include "stargate.h"

#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "Main/mapper.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Defcon::CStargate::CStargate()
{
	m_parentType = m_type;
	m_type = ObjType::STARGATE;
	m_smallColor.A = 0.0f;// we'll draw the gate in the radar screen ourselves   = C_BLUE;
	m_bInjurious = false;
	m_bCanBeInjured = false;



	const int cby[] = 
		{
			CGameColors::gray, 
			CGameColors::red,
			CGameColors::yellow,
			CGameColors::blue,
			CGameColors::lightyellow,
			CGameColors::lightblue
		};

	m_bMortal = false;

	CreateSprite(ObjType::STARGATE);

	m_bboxrad.Set(Sprite->GetActualSize().X / 2, Sprite->GetActualSize().Y / 2);
}


#ifdef _DEBUG
const char* Defcon::CStargate::GetClassname() const
{
	static char* psz = "Stargate";
	return psz;
}
#endif



void Defcon::CStargate::Move(float f)
{
	m_fAge += f;
}


void Defcon::CStargate::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CStargate::DrawSmallPart(FPaintArguments& framebuf, const I2DCoordMapper& mapper, FSlateBrush& Brush, const CFPoint& TopLeft, const CFPoint& BottomRight, const CFPoint& BoxRad)
{
	if(ABS(TopLeft.x - BottomRight.x) > BoxRad.x * 3)
	{
		// The rectangle intersects the screen edge, so draw two rectangles.

		// Turns out this statement executes in both cases.
		framebuf.ColorRect(0, TopLeft.y, BottomRight.x, BottomRight.y, C_BLUE);

		const float ScreenWidth = framebuf.AllottedGeometry->GetLocalSize().X;

		if(TopLeft.x < 0)
		{
			// Draw left half at right edge of screen
			framebuf.ColorRect(ScreenWidth + TopLeft.x, TopLeft.y, ScreenWidth, BottomRight.y, C_BLUE);
		}
		else
		{
			// Draw right half at right edge of screen
			framebuf.ColorRect(TopLeft.x, TopLeft.y, ScreenWidth, BottomRight.y, C_BLUE);
		}

		return;
	}

	framebuf.ColorRect(TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y, C_BLUE);
}


void Defcon::CStargate::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& mapper, FSlateBrush& Brush)
{
	// Draw the gate using two rectangles.
	// Note: we have to draw it twice if it is split across a horizontal edge.

	CFPoint TopLeft, BottomRight;

	mapper.To(m_pos - m_bboxrad, TopLeft);
	mapper.To(m_pos + m_bboxrad, BottomRight);

	DrawSmallPart(framebuf, mapper, Brush, TopLeft, BottomRight, m_bboxrad);

	// Draw small rect inside large one
	auto box = m_bboxrad;
	// Make a small box centered inside the large one
	box *= CFPoint(0.3f, 0.375f);
	auto TL = m_pos - box;
	auto BR = m_pos + box;

	// Shift the box over a bit.
	TL.x -= m_bboxrad.x / 3;
	BR.x -= m_bboxrad.x / 3;

	mapper.To(TL, TopLeft);
	mapper.To(BR, BottomRight);

	DrawSmallPart(framebuf, mapper, Brush, TopLeft, BottomRight, box);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
