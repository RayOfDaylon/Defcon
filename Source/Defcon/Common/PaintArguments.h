// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Runtime/SlateCore/Public/Layout/SlateRect.h"
#include "Runtime/SlateCore/Public/Types/PaintArgs.h"
#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"
#include "Runtime/SlateCore/Public/Styling/WidgetStyle.h"

#include "util_color.h"


struct FPaintArguments
{
	// Anything that issues draw calls directly takes a reference to this structure e.g. GameObject::Draw

	const FPaintArgs*        Args;
	const FGeometry*         AllottedGeometry;
	const FPaintGeometry*    PaintGeometry;
	const FSlateRect*        MyCullingRect;
	FSlateWindowElementList* OutDrawElements;
	int32                    LayerId;
	bool                     bParentEnabled;
	float                    RenderOpacity;

	float GetWidth  () const { return AllottedGeometry->GetAbsoluteSize().X / AllottedGeometry->Scale; }
	float GetHeight () const { return AllottedGeometry->GetAbsoluteSize().Y / AllottedGeometry->Scale; }

	void DrawLaserBeam         (int32 X, int32 Y, int32 X2, const FLinearColor& Color);
	void ColorRect             (float Left, float Top, float Right, float Bottom, const FLinearColor& Color, float Linewidth = 2.0f);
	void FillRect              (float Left, float Top, float Right, float Bottom, const FLinearColor& Color);
	void ColorStraightLine     (int32 X, int32 Y, int32 W, int32 H, const FLinearColor& Color) {}
	void BrightenStraightLine  (int32 X, int32 Y, int32, int32) {}
	//void DarkenRect            (int32 Left, int32 Top, int32 Right, int32 Bottom, float Amount) {}
};
