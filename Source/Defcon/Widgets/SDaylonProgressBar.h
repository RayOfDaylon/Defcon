// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Attribute.h"
#include "Styling/SlateColor.h"
#include "Widgets/SLeafWidget.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "Runtime/Core/Public/Math/Color.h"
//#include "Styling/SlateWidgetStyleAsset.h"
//#include "Widgets/DeclarativeSyntaxSupport.h"
//#include "Styling/SlateTypes.h"
//#include "Styling/CoreStyle.h"
//#include "SDaylonProgressBar.generated.h"

class FPaintArgs;
//class FSlateWindowElementList;


// A simple progress bar to work around bugs in UE
class DEFCON_API SDaylonProgressBar : public SLeafWidget
{
	public:
	
		SLATE_BEGIN_ARGS(SDaylonProgressBar)
			:
			  _Percent( 0.0f )
			, _BorderThickness( 0.0f)
			, _FillColorAndOpacity( FLinearColor::White )
			{
				_Visibility = EVisibility::SelfHitTestInvisible;
			}

			SLATE_ATTRIBUTE( float, Percent )

			SLATE_ATTRIBUTE( float, BorderThickness )

			SLATE_ATTRIBUTE( FSlateColor, FillColorAndOpacity )

		SLATE_END_ARGS()

		/**
		 * Construct the widget
		 * 
		 * @param InArgs   A declaration from which to construct the widget
		 */
		void Construct(const FArguments& InArgs);

		virtual int32 OnPaint(
			const FPaintArgs& Args, 
			const FGeometry& AllottedGeometry, 
			const FSlateRect& MyCullingRect, 
			FSlateWindowElementList& OutDrawElements, 
			int32 LayerId, 
			const FWidgetStyle& InWidgetStyle, 
			bool bParentEnabled) const override;

		virtual FVector2D ComputeDesiredSize(float) const override;
		//virtual bool ComputeVolatility() const override;

		void SetPercent              (TAttribute<float>       InPercent);
		void SetFillColorAndOpacity  (TAttribute<FSlateColor> InFillColorAndOpacity);
		void SetBorderThickness      (TAttribute<float>       InBorderThickness);
	
	
	protected:
	
		FSlateColorBrush           Brush = FSlateColorBrush(FLinearColor::White);
		TAttribute<float>          Percent;
		TAttribute<FSlateColor>    FillColorAndOpacity;
		FLinearColor               ActualColor = FLinearColor::White;
		TAttribute<float>          BorderThickness;
};

