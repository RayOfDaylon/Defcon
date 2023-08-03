// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "Painter.h"
#include "util_math.h"
#include "util_color.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"


void FPainter::DrawLaserBeam(float X, float Y, float X2, const FLinearColor& Color)
{
	TArray<FVector2f> Points;

	Points.Add(FVector2f(X,  Y));
	Points.Add(FVector2f(X2, Y));

	FSlateDrawElement::MakeLines(*OutDrawElements, LayerId, *PaintGeometry, Points, ESlateDrawEffect::None, Color, true, 2.0f);
}


void FPainter::ColorRect(float Left, float Top, float Right, float Bottom, const FLinearColor& Color, float Linewidth)
{
	TArray<FVector2f> Points;

	Points.Add(FVector2f(Left,  Top));
	Points.Add(FVector2f(Right, Top));
	Points.Add(FVector2f(Right, Bottom));
	Points.Add(FVector2f(Left,  Bottom));
	Points.Add(FVector2f(Left,  Top));

	FSlateDrawElement::MakeLines(*OutDrawElements, LayerId, *PaintGeometry, Points, ESlateDrawEffect::None, Color, true, Linewidth);
}


void FPainter::FillRect(float Left, float Top, float Right, float Bottom, const FLinearColor& Color)
{
	const auto S = FVector2D(ABS(Right - Left), ABS(Bottom - Top));
	const FSlateLayoutTransform Translation(FVector2D(AVG(Left, Right), AVG(Top, Bottom)) - S / 2);
	//const FSlateLayoutTransform Translation(FVector2D(0, 0));

	const auto Geometry = AllottedGeometry->MakeChild(S, Translation);

	const FSlateColorBrush Brush(C_WHITE);

	FSlateDrawElement::MakeBox(
		*OutDrawElements,
		LayerId,
		Geometry.ToPaintGeometry(),
		&Brush,
		ESlateDrawEffect::None,
		Color * RenderOpacity);
}


