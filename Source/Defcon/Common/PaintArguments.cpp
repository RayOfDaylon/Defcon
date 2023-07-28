// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "PaintArguments.h"
#include "util_math.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"


void FPaintArguments::DrawLaserBeam(int32 X, int32 Y, int32 X2, const FLinearColor& Color)
{
	TArray<FVector2f> Points;

	Points.Add(FVector2f(X,  Y));
	Points.Add(FVector2f(X2, Y));

	FSlateDrawElement::MakeLines(*OutDrawElements, LayerId, *PaintGeometry, Points, ESlateDrawEffect::None, Color, true, 2.0f);
}


void FPaintArguments::ColorRect(float Left, float Top, float Right, float Bottom, const FLinearColor& Color, float Linewidth)
{
	TArray<FVector2f> Points;

	Points.Add(FVector2f(Left,  Top));
	Points.Add(FVector2f(Right, Top));
	Points.Add(FVector2f(Right, Bottom));
	Points.Add(FVector2f(Left,  Bottom));
	Points.Add(FVector2f(Left,  Top));

	FSlateDrawElement::MakeLines(*OutDrawElements, LayerId, *PaintGeometry, Points, ESlateDrawEffect::None, Color, true, Linewidth);
}


void FPaintArguments::FillRect(float Left, float Top, float Right, float Bottom, const FLinearColor& Color)
{
	const auto S = FVector2D(ABS(Right - Left), ABS(Bottom - Top));
	const FSlateLayoutTransform Translation(FVector2D((Left + Right) / 2, (Top + Bottom) / 2) - S / 2);
	//const FSlateLayoutTransform Translation(FVector2D(0, 0));

	const auto Geometry = AllottedGeometry->MakeChild(S, Translation);

	const FSlateColorBrush Brush(C_WHITE);

	FSlateDrawElement::MakeBox(
		*OutDrawElements,
		LayerId,
		Geometry.ToPaintGeometry(),
		&Brush,
		ESlateDrawEffect::None,
		Color * InWidgetStyle->GetColorAndOpacityTint().A);
}


