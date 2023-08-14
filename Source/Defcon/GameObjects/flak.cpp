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

	BrushPtr = FRAND > 0.1f ? GGameObjectResources.DebrisBrushRoundPtr : GGameObjectResources.DebrisBrushSquarePtr;

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


void Defcon::CFlak::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	// Determine a color, Size, and shape for a debris piece.

	CFPoint Pt;
	Mapper.To(Position, Pt);

	float Youth = Lifespan / MaxAge;

	const float Size = FRANDRANGE(0.5f, 1.0f) * Youth * LargestSize + 1;

	if(Pt.x >= -10 && Pt.x < Painter.GetWidth() + 10)
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
			// Particle is old.

			Color = IRAND(4) != 0 ? GGameColors.GetColor(ColorbaseOld, Youth) : GGameColors.GetColor(EColor::Red, Youth);
		}
		else
		{
			// Particle is young.
			Color = IRAND(4) != 0 ? 
				(BRAND ? C_WHITE : GGameColors.GetColor(ColorbaseYoung, Youth))
				: GGameColors.GetColor((Defcon::EColor)IRAND(3), Youth);
		}

		const auto S = FVector2D(R.Width(), R.Height());
		const FSlateLayoutTransform Translation(FVector2D(R.Left, R.Top));
		const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*Painter.OutDrawElements,
			Painter.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * Painter.RenderOpacity);
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


void Defcon::CGlowingFlak::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	CFPoint Pt;
	Mapper.To(Position, Pt);

	const float Youth = Lifespan / MaxAge;
	const float Size  = FRANDRANGE(0.5f, 1.0f) * Youth * LargestSize + 1.0f;

	if(Pt.x >= -10 && Pt.x < Painter.GetWidth() + 10)
	{
		auto Color = C_WHITE;
		const auto S = FVector2D(Size, Size);
		const FSlateLayoutTransform Translation(FVector2D(Pt.x, Pt.y) - S / 2);
		const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*Painter.OutDrawElements,
			Painter.LayerId,
			Geometry.ToPaintGeometry(),
			BrushPtr,
			ESlateDrawEffect::None,
			Color * Painter.RenderOpacity);
	}
}


#undef BUDGE
