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
	m_eColorbaseOld(CGameColors::red),
	m_eColorbaseYoung(CGameColors::gray),
	m_bCold(false)
{
	m_parentType = m_type;
	m_type = ObjType::FLAK;
	m_bMortal = true;
	m_bInjurious = false;
	m_bCanBeInjured = false;

	m_maxAge = m_fLifespan = Daylon::FRandRange(0.5f, 2.0f);

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


void Defcon::CFlak::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	// Determine a color, size, and shape for a debris piece.

	CFPoint pt;
	mapper.To(m_pos, pt);

	float fYouth = m_fLifespan / m_maxAge;

	const int32 size = (int32)((FRAND * 0.5f + 0.5f) * fYouth * /*8*/	m_fLargestSize + 1);

	if(pt.x >= -10 && pt.x < framebuf.GetWidth() + 10)
	{
		#define BUDGE (IRAND(3) - 1)

		IntRect r;
		r.left   = ROUND(pt.x - size + BUDGE);
		r.right  = ROUND(pt.x + size + BUDGE);
		r.top    = ROUND(pt.y - size + BUDGE);
		r.bottom = ROUND(pt.y + size + BUDGE);


		// values of the CGameColors enum
		// 			int		m_eColorbaseOld;
		//	int		m_eColorbaseYoung;

		if(!m_bFade)
		{
			fYouth = 1.0f;
		}

		FLinearColor Color;

		if(fYouth < 0.5f || m_bCold)
		{
			// flak piece is old.

			Color = IRAND(4) != 0 ? gGameColors.GetColor(m_eColorbaseOld, fYouth) : gGameColors.GetColor(CGameColors::red, fYouth);
		}
		else
		{
			// flak piece is young.
			Color = IRAND(4) != 0 ? 
				(BRAND ? C_WHITE : gGameColors.GetColor(m_eColorbaseYoung /*m_eColorbaseOld*/, fYouth))
				: gGameColors.GetColor(IRAND(3), fYouth);
		}

		const auto S = FVector2D(r.width(), r.height());
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*framebuf.OutDrawElements,
			framebuf.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * framebuf.RenderOpacity);
	}
}


void Defcon::CFlak::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& map, FSlateBrush&)
{
}


void Defcon::CFlak::Move(float fElapsedTime)
{
	m_pos.muladd(m_orient.fwd, fElapsedTime);
}


// ------------------------------------------------------

#ifdef _DEBUG
const char* Defcon::CGlowingFlak::GetClassname() const
{
	static char* psz = "Glowing_Flak";
	return psz;
}
#endif


void Defcon::CGlowingFlak::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	CFPoint pt;
	mapper.To(m_pos, pt);

	float fYouth = m_fLifespan / m_maxAge;

	int size = (int)((FRAND * 0.5f + 0.5f) * fYouth * /*8*/	m_fLargestSize + 1);

	int halfsize = size / 2;

	if(pt.x >= -10 && pt.x < framebuf.GetWidth() + 10)
	{

#define BUDGE (IRAND(3) - 1)

#if 1
		//FLinearColor color;
		/*CTrueBitmap& bmp = gBitmaps.GetBitmap(
			IRAND(10) == 0
			? CBitmaps::fireball0 + IRAND(3)
			: CBitmaps::bullet5x5 + IRAND(2));

		int w = bmp.GetWidth();
		if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
		{
			pt.sub(CFPoint((float)w/2, (float)bmp.GetHeight()/2));
			bmp.BlitAlphaBrighten(
				framebuf, ROUND(pt.x), ROUND(pt.y), 
				w, bmp.GetHeight(), 
				0, 0, fYouth);
		}*/

		auto Color = C_WHITE;
		const auto S = FVector2D(size, size);
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*framebuf.OutDrawElements,
			framebuf.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * framebuf.RenderOpacity);


#else		
		{
			if(!m_bFade)
				fYouth = 1.0f;
			if(fYouth < 0.5f || m_bCold)
			{
				// flak piece is old.
				color = (
					rand()%4 ? 
					gGameColors.GetColor(m_eColorbaseOld, fYouth) :
					gGameColors.GetColor(CGameColors::red, fYouth));

			}
			else
			{
				// flak piece is young.
				color = (
					rand()%4 ? 
					(BRAND ? C_WHITE :
					gGameColors.GetColor(m_eColorbaseYoung, fYouth) ) :
					gGameColors.GetColor(rand()%3, fYouth));

			}
		}
		if(false || IRAND(7) > 0)
		{
			//Rectangle_(dc, r.left, r.top, r.right, r.bottom);
			//framebuf.FillRect(r.left, r.top, r.right, r.bottom);
			CMaskMap& mask = gBitmaps.GetMask(size+4, IRAND(3));
			//Ellipse_(dc, r.left, r.top, r.right, r.bottom);
			mask.ColorWith(color, framebuf, ROUND(pt.x)-halfsize-2, ROUND(pt.y)-halfsize-2); 
		}
		if(IRAND(3) == 0)
		{
			IntRect r;
			r.left = ROUND(pt.x - size + BUDGE);
			r.right = ROUND(pt.x + size + BUDGE);
			r.top = ROUND(pt.y - size + BUDGE);
			r.bottom = ROUND(pt.y + size + BUDGE);
			framebuf.FillRect(r.left, r.top, r.right, r.bottom, color);
		}
		else
		{
			CMaskMap& mask = gBitmaps.GetMask(size+1, IRAND(3));
			//Ellipse_(dc, r.left, r.top, r.right, r.bottom);
			mask.ColorWith(color, framebuf, ROUND(pt.x)-halfsize, ROUND(pt.y)-halfsize); 
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
	m_bMortal = true;
	m_bInjurious = false;
	m_bCanBeInjured = false;

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
	FPaintArguments& framebuf, 
	const I2DCoordMapper& mapper
)
{
	CFPoint pt;
	mapper.To(m_pos, pt);

	float fYouth = m_fLifespan / m_maxAge;

	// Make puff grow from small to large quickly 
	// at the start, and then shrink to small slowly.
	int size;
	if(fYouth >= 0.95f)
	{
		float t = CLAMP(fYouth, 0.95f, 1.0f);
		t = NORM_(t, 0.95f, 1.0f);
		
		size = (int)(LERP(m_fLargestSize, 0.0f, t));
	}
	else
		size = (int)((FRAND*.05f+.95f) * fYouth * 
						m_fLargestSize);

	size++;

	if(pt.x >= -10 && pt.x < framebuf.GetWidth() + 10)
	{
		HDC_ dc = framebuf.GetDC();
		HPEN_ orgpen = (HPEN_)SelectGdiObject_(dc, GetStockObject_(NULL_PEN_));

#define _BUDGE 0
//(rand() % 3 - 1)

		float stretch = ((1.0f - fYouth))*6+1.5f;
		IntRect r;
		r.left = ROUND(pt.x - size*stretch + _BUDGE);
		r.right = ROUND(pt.x + size*stretch + _BUDGE);
		r.top = ROUND(pt.y - size + _BUDGE);
		r.bottom = ROUND(pt.y + size + _BUDGE);

		int rop = SetROP2_(dc, R2_MERGEPEN_);

		HBRUSH_ orgbr;
		
		orgbr = (HBRUSH_)SelectGdiObject_(
			dc, 
			gGameColors.GetBrush(CGameColors::gray, m_fBrightest*fYouth*0.5f));		

		Ellipse_(dc, r.left, r.top, r.right, r.bottom);

		(void)SelectGdiObject_(dc, orgpen);
		(void)SelectGdiObject_(dc, orgbr);
		SetROP2_(dc, rop);
	}

}


void Defcon::CPuff::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& map, FSlateBrush&)
{
	// Smoke doesn't appear on radar.
}



void Defcon::CPuff::Move(float fElapsedTime)
{
	m_pos.muladd(m_orient.fwd, fElapsedTime);
}

#endif
