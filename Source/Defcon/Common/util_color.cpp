// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/* 
	util_color.cpp
	Color-related types and routines. 
*/

#include "util_color.h"


FLinearColor MakeBlendedColor(const FLinearColor& ColorA, const FLinearColor& ColorB, float T)
{
	FLinearColor Result;
	LerpColor(ColorA, ColorB, T, Result);

	return Result;
}


FLinearColor MakeColorFromComponents(int32 Red, int32 Green, int32 Blue)
{
	return FLinearColor(Red / 255.0f, Green / 255.0f, Blue / 255.0f, 1.0f);
}


void LerpColor(const FLinearColor& ColorA, const FLinearColor& ColorB, float T, FLinearColor& Result)
{
	// UE only has a LerpUsingHSV for FLinearColor, so here we support plain lerp.

	// todo: move to plugin.

	Result.R = FMath::Lerp(ColorA.R, ColorB.R, T);
	Result.G = FMath::Lerp(ColorA.G, ColorB.G, T);
	Result.B = FMath::Lerp(ColorA.B, ColorB.B, T);
	Result.A = FMath::Lerp(ColorA.A, ColorB.A, T);
}
