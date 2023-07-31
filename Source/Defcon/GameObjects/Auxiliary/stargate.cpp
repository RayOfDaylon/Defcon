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
	ParentType = Type;
	Type = EObjType::STARGATE;
	RadarColor.A = 0.0f;// we'll draw the gate in the radar screen ourselves   = C_BLUE;
	bInjurious = false;
	bCanBeInjured = false;



	const int cby[] = 
		{
			CGameColors::gray, 
			CGameColors::red,
			CGameColors::yellow,
			CGameColors::blue,
			CGameColors::lightyellow,
			CGameColors::lightblue
		};

	bMortal = false;

	CreateSprite(EObjType::STARGATE);

	BboxRadius.Set(Sprite->GetActualSize().X / 2, Sprite->GetActualSize().Y / 2);
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
	Age += f;
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

	mapper.To(Position - BboxRadius, TopLeft);
	mapper.To(Position + BboxRadius, BottomRight);

	DrawSmallPart(framebuf, mapper, Brush, TopLeft, BottomRight, BboxRadius);

	// Draw small rect inside large one
	auto box = BboxRadius;
	// Make a small box centered inside the large one
	box *= CFPoint(0.3f, 0.375f);
	auto TL = Position - box;
	auto BR = Position + box;

	// Shift the box over a bit.
	TL.x -= BboxRadius.x / 3;
	BR.x -= BboxRadius.x / 3;

	mapper.To(TL, TopLeft);
	mapper.To(BR, BottomRight);

	DrawSmallPart(framebuf, mapper, Brush, TopLeft, BottomRight, box);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
