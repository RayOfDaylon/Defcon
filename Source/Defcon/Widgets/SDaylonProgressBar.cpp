// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "SDaylonProgressBar.h"
#include "Rendering/DrawElements.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



void SDaylonProgressBar::Construct( const FArguments& InArgs )
{
	SetPercent(InArgs._Percent);
	SetFillColorAndOpacity(InArgs._FillColorAndOpacity);
	SetBorderThickness(InArgs._BorderThickness);

	SetCanTick(false);
}


void SDaylonProgressBar::SetPercent(TAttribute<float> InPercent)
{
	if (!Percent.IdenticalTo(InPercent))
	{
		Percent = InPercent;
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}


void SDaylonProgressBar::SetFillColorAndOpacity(TAttribute<FSlateColor> InFillColorAndOpacity)
{
	if(!FillColorAndOpacity.IdenticalTo(InFillColorAndOpacity))
	{
		FillColorAndOpacity = InFillColorAndOpacity;
		FWidgetStyle Style = FWidgetStyle();
		ActualColor = FLinearColor(FillColorAndOpacity.Get().GetColor(Style));
		Invalidate(EInvalidateWidget::Paint);
	}
}


void SDaylonProgressBar::SetBorderThickness(TAttribute<float> InBorderThickness)
{
	if (!BorderThickness.IdenticalTo(InBorderThickness))
	{
		BorderThickness = InBorderThickness;
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}


int32 SDaylonProgressBar::OnPaint
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
	if(!Percent.IsSet())
	{
		return LayerId;
	}

	const float ProgressFraction = Percent.Get();	

	const FPaintGeometry PaintRect = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2f(ProgressFraction, 1.0f)), FVector2D(0.0f, 0.5f)).ToPaintGeometry();

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		PaintRect,
		&Brush,
		ESlateDrawEffect::None,
		ActualColor * InWidgetStyle.GetColorAndOpacityTint()
	);
	
	return LayerId;
}

FVector2D SDaylonProgressBar::ComputeDesiredSize( float ) const
{
	return Brush.ImageSize;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
