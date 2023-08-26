// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_color.h
	Color-related types and routines. 
*/

#include "CoreMinimal.h"
#include "Runtime/Core/Public/Math/Color.h"


constexpr auto C_WHITE        = FLinearColor(1.0f,   1.0f,   1.0f,   1.0f);
constexpr auto C_BRIGHT       = FLinearColor(0.875f, 0.875f, 0.875f, 1.0f);
constexpr auto C_LIGHT        = FLinearColor(0.75f,  0.75f,  0.75f,  1.0f);
constexpr auto C_DARK         = FLinearColor(0.5f,   0.5f,   0.5f,   1.0f);
constexpr auto C_DARKER       = FLinearColor(0.25f,  0.25f,  0.25f,  1.0f);
constexpr auto C_BLACK        = FLinearColor(0.0f,   0.0f,   0.0f,   1.0f);

constexpr auto C_RED          = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
constexpr auto C_GREEN        = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
constexpr auto C_BLUE         = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);

constexpr auto C_DARKBLUE     = FLinearColor(0.0f,   0.0f,   0.5f,  1.0f);
constexpr auto C_DARKGREEN    = FLinearColor(0.0f,   0.5f,   0.0f,  1.0f);
constexpr auto C_DARKRED      = FLinearColor(0.5f,   0.0f,   0.0f,  1.0f);

constexpr auto C_LIGHTERBLUE  = FLinearColor(0.75f, 0.75f, 1.0f, 1.0f);
constexpr auto C_LIGHTBLUE    = FLinearColor(0.5f,  0.5f,  1.0f, 1.0f);
constexpr auto C_LIGHTRED     = FLinearColor(1.0f,  0.5f,  0.5f, 1.0f);
constexpr auto C_LIGHTGREEN   = FLinearColor(0.5f,  1.0f,  0.5f, 1.0f);

constexpr auto C_YELLOW       = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
constexpr auto C_MAGENTA      = FLinearColor(1.0f, 0.0f, 1.0f, 1.0f); // red + blue
constexpr auto C_CYAN         = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // green + blue
constexpr auto C_ORANGE       = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // half green + red

constexpr auto C_DARKYELLOW   = FLinearColor(0.5f, 0.5f, 0.0f, 1.0f);
constexpr auto C_BROWN        = C_DARKYELLOW;
constexpr auto C_DARKMAGENTA  = FLinearColor(0.5f, 0.0f, 0.5f, 1.0f);
constexpr auto C_DARKCYAN     = FLinearColor(0.0f, 0.5f, 0.5f, 1.0f);

constexpr auto C_LIGHTYELLOW  = FLinearColor(1.0f, 1.0f,  0.5f, 1.0f);
constexpr auto C_LIGHTMAGENTA = FLinearColor(1.0f, 0.5f,  1.0f, 1.0f);
constexpr auto C_LIGHTCYAN    = FLinearColor(0.5f, 1.0f,  1.0f, 1.0f);
constexpr auto C_LIGHTORANGE  = FLinearColor(1.0f, 0.75f, 0.5f, 1.0f);

constexpr auto C_HALFDARKRED  = FLinearColor(0.75f, 0.0f, 0.0f, 1.0f);


void         LerpColor               (const FLinearColor& ColorA, const FLinearColor& ColorB, float T, FLinearColor& Result);
FLinearColor MakeBlendedColor        (const FLinearColor& ColorA, const FLinearColor& ColorB, float T);
FLinearColor MakeColorFromComponents (int32 Red, int32 Green, int32 Blue);

