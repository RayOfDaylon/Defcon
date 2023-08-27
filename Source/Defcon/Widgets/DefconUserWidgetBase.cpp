// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconUserWidgetBase.h"


#if 0
void UDefconCellExpanderBase::UpdateBorderWidth()
{
	const auto& Geometry = GetPaintSpaceGeometry();

	//return Geometry.GetAbsoluteSize() / Geometry.Scale;

	const auto W = Geometry.Scale * 0.125f;

	//Border->>GetSlateBrush


}
#endif


int32 UDefconCellExpanderBase::NativePaint
(
	const FPaintArgs&        Args, 
	const FGeometry&         AllottedGeometry, 
	const FSlateRect&        MyCullingRect, 
	FSlateWindowElementList& OutDrawElements, 
	int32                    LayerId, 
	const FWidgetStyle&      InWidgetStyle, 
	bool                     bParentEnabled
) const
{
	// Draw four lines of width BorderWidth to make a rect
	
	// Inset the size to not draw linesr centered on widget border.

	const float HalfW = BorderWidth / 2;

	const auto S = AllottedGeometry.GetLocalSize() - FVector2D(BorderWidth);

	TArray<FVector2f> LinePts;

	LinePts.Add(FVector2f(HalfW,    HalfW));
	LinePts.Add(FVector2f(S.X,      HalfW));
	LinePts.Add(FVector2f(S.X,      S.Y));
	LinePts.Add(FVector2f(HalfW,    S.Y));
	LinePts.Add(FVector2f(HalfW,    HalfW));
	LinePts.Add(FVector2f(S.X,      HalfW)); // "Round the corner" for the first point to make sure it looks sealed.

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), LinePts, ESlateDrawEffect::None, BorderColor, true, BorderWidth);

	return LayerId;
}

