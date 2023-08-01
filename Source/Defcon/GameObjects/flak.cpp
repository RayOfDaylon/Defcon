// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/* 
	flak.cpp
	Implements debris entities in Defcon game.
*/


#include "flak.h"

#include "Common/util_color.h"
#include "Globals/GameObjectResources.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CFlak::CFlak()
	:
	ColorbaseOld(EColor::Red),
	ColorbaseYoung(EColor::Gray),
	bCold(false)
{
	ParentType = Type;
	Type       = EObjType::FLAK;

	bMortal       = true;
	bInjurious    = false;
	bCanBeInjured = false;

	MaxAge = Lifespan = Daylon::FRandRange(0.5f, 2.0f);

	BrushPtr = FRAND > 0.1f ? GameObjectResources.DebrisBrushRoundPtr : GameObjectResources.DebrisBrushSquarePtr;

	check(BrushPtr != nullptr);
}


Defcon::CFlak::~CFlak()
{
}


#ifdef _DEBUG
const char* Defcon::CFlak::GetClassname() const
{
	static char* psz = "Flak";
	return psz;
}
#endif


void Defcon::CFlak::Draw(FPaintArguments& PaintArgs, const I2DCoordMapper& Mapper)
{
	// Determine a color, Size, and shape for a debris piece.

	CFPoint Pt;
	Mapper.To(Position, Pt);

	float Youth = Lifespan / MaxAge;

	const int32 Size = (int32)((FRAND * 0.5f + 0.5f) * Youth * /*8*/	LargestSize + 1);

	if(Pt.x >= -10 && Pt.x < PaintArgs.GetWidth() + 10)
	{
		// Make our display quad vary each frame in size and aspect.

		#define BUDGE FRANDRANGE(-2.0f, 2.0f)

		IntRect R;
		R.Left   = ROUND(Pt.x - Size + BUDGE);
		R.Right  = ROUND(Pt.x + Size + BUDGE);
		R.Top    = ROUND(Pt.y - Size + BUDGE);
		R.Bottom = ROUND(Pt.y + Size + BUDGE);

		if(!bFade)
		{
			Youth = 1.0f;
		}

		FLinearColor Color;

		if(Youth < 0.5f || bCold)
		{
			// flak piece is old.

			Color = IRAND(4) != 0 ? gGameColors.GetColor(ColorbaseOld, Youth) : gGameColors.GetColor(EColor::Red, Youth);
		}
		else
		{
			// flak piece is young.
			Color = IRAND(4) != 0 ? 
				(BRAND ? C_WHITE : gGameColors.GetColor(ColorbaseYoung, Youth))
				: gGameColors.GetColor((Defcon::EColor)IRAND(3), Youth);
		}

		const auto S = FVector2D(R.Width(), R.Height());
		const FSlateLayoutTransform Translation(FVector2D(R.Left, R.Top));
		const auto Geometry = PaintArgs.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*PaintArgs.OutDrawElements,
			PaintArgs.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * PaintArgs.RenderOpacity);
	}
}


void Defcon::CFlak::Move(float DeltaTime)
{
	Position.MulAdd(Orientation.Fwd, DeltaTime);
}


// ------------------------------------------------------

#ifdef _DEBUG
const char* Defcon::CGlowingFlak::GetClassname() const
{
	static char* psz = "Glowing_Flak";
	return psz;
}
#endif


void Defcon::CGlowingFlak::Draw(FPaintArguments& PaintArgs, const I2DCoordMapper& Mapper)
{
	CFPoint Pt;
	Mapper.To(Position, Pt);

	float Youth = Lifespan / MaxAge;

	int Size = (int)((FRAND * 0.5f + 0.5f) * Youth * /*8*/	LargestSize + 1);

	int halfsize = Size / 2;

	if(Pt.x >= -10 && Pt.x < PaintArgs.GetWidth() + 10)
	{


#if 1
		//FLinearColor color;
		/*CTrueBitmap& bmp = gBitmaps.GetBitmap(
			IRAND(10) == 0
			? CBitmaps::fireball0 + IRAND(3)
			: CBitmaps::bullet5x5 + IRAND(2));

		int w = bmp.GetWidth();
		if(Pt.x >= -w && Pt.x <= PaintArgs.GetWidth() + w)
		{
			Pt.sub(CFPoint((float)w/2, (float)bmp.GetHeight()/2));
			bmp.BlitAlphaBrighten(
				PaintArgs, ROUND(Pt.x), ROUND(Pt.y), 
				w, bmp.GetHeight(), 
				0, 0, Youth);
		}*/

		auto Color = C_WHITE;
		const auto S = FVector2D(Size, Size);
		const FSlateLayoutTransform Translation(FVector2D(Pt.x, Pt.y) - S / 2);
		const auto Geometry = PaintArgs.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*PaintArgs.OutDrawElements,
			PaintArgs.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * PaintArgs.RenderOpacity);


#else		
		{
			if(!bFade)
				Youth = 1.0f;
			if(Youth < 0.5f || bCold)
			{
				// flak piece is old.
				color = (
					rand()%4 ? 
					gGameColors.GetColor(ColorbaseOld, Youth) :
					gGameColors.GetColor(EColor::Red, Youth));

			}
			else
			{
				// flak piece is young.
				color = (
					rand()%4 ? 
					(BRAND ? C_WHITE :
					gGameColors.GetColor(ColorbaseYoung, Youth) ) :
					gGameColors.GetColor(rand()%3, Youth));

			}
		}
		if(false || IRAND(7) > 0)
		{
			//Rectangle_(dc, R.left, R.top, R.right, R.bottom);
			//PaintArgs.FillRect(R.left, R.top, R.right, R.bottom);
			CMaskMap& mask = gBitmaps.GetMask(Size+4, IRAND(3));
			//Ellipse_(dc, R.left, R.top, R.right, R.bottom);
			mask.ColorWith(color, PaintArgs, ROUND(Pt.x)-halfsize-2, ROUND(Pt.y)-halfsize-2); 
		}
		if(IRAND(3) == 0)
		{
			IntRect R;
			R.left = ROUND(Pt.x - Size + BUDGE);
			R.right = ROUND(Pt.x + Size + BUDGE);
			R.top = ROUND(Pt.y - Size + BUDGE);
			R.bottom = ROUND(Pt.y + Size + BUDGE);
			PaintArgs.FillRect(R.left, R.top, R.right, R.bottom, color);
		}
		else
		{
			CMaskMap& mask = gBitmaps.GetMask(Size+1, IRAND(3));
			//Ellipse_(dc, R.left, R.top, R.right, R.bottom);
			mask.ColorWith(color, PaintArgs, ROUND(Pt.x)-halfsize, ROUND(Pt.y)-halfsize); 
		}
#endif
	}
}

#if 0
// ------------------------------------------------------

Defcon::CPuff::CPuff()
	:
	m_fBrightest(1.0f)
{
	bMortal = true;
	bInjurious = false;
	bCanBeInjured = false;

}


Defcon::CPuff::~CPuff()
{
}


#ifdef _DEBUG
const char* Defcon::CPuff::GetClassname() const
{
	static char* psz = "Flak";
	return psz;
}
#endif

void Defcon::CPuff::Draw
(
	FPaintArguments& PaintArgs, 
	const I2DCoordMapper& Mapper
)
{
	CFPoint Pt;
	Mapper.To(Position, Pt);

	float Youth = Lifespan / MaxAge;

	// Make puff grow from small to large quickly 
	// at the start, and then shrink to small slowly.
	int Size;
	if(Youth >= 0.95f)
	{
		float t = CLAMP(Youth, 0.95f, 1.0f);
		t = NORM_(t, 0.95f, 1.0f);
		
		Size = (int)(LERP(LargestSize, 0.0f, t));
	}
	else
		Size = (int)((FRAND*.05f+.95f) * Youth * 
						LargestSize);

	Size++;

	if(Pt.x >= -10 && Pt.x < PaintArgs.GetWidth() + 10)
	{
		HDC_ dc = PaintArgs.GetDC();
		HPEN_ orgpen = (HPEN_)SelectGdiObject_(dc, GetStockObject_(NULL_PEN_));

#define _BUDGE 0
//(rand() % 3 - 1)

		float stretch = ((1.0f - Youth))*6+1.5f;
		IntRect R;
		R.left = ROUND(Pt.x - Size*stretch + _BUDGE);
		R.right = ROUND(Pt.x + Size*stretch + _BUDGE);
		R.top = ROUND(Pt.y - Size + _BUDGE);
		R.bottom = ROUND(Pt.y + Size + _BUDGE);

		int rop = SetROP2_(dc, R2_MERGEPEN_);

		HBRUSH_ orgbr;
		
		orgbr = (HBRUSH_)SelectGdiObject_(
			dc, 
			gGameColors.GetBrush(EColor::Gray, m_fBrightest*Youth*0.5f));		

		Ellipse_(dc, R.left, R.top, R.right, R.bottom);

		(void)SelectGdiObject_(dc, orgpen);
		(void)SelectGdiObject_(dc, orgbr);
		SetROP2_(dc, rop);
	}

}


void Defcon::CPuff::DrawSmall(FPaintArguments& PaintArgs, const I2DCoordMapper& map, FSlateBrush&)
{
	// Smoke doesn't appear on radar.
}



void Defcon::CPuff::Move(float DeltaTime)
{
	Position.MulAdd(Orientation.Fwd, DeltaTime);
}

#endif

#undef BUDGE
