// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "GameObjects/terrain.h"
#include "SlateCore/Public/Widgets/SLeafWidget.h"




// SDefconTerrain - a Slate widget with a custom Paint event.
// Used to draw the terrain for the Defense Condition game.
// As a widget, we can ensure it draws behind other widgets.

class DEFCON_API SDefconTerrain : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDefconTerrain)
			: 
			  _Size                (FVector2D(0, 0))
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D, Size)

			SLATE_END_ARGS()

			SDefconTerrain() {}

			~SDefconTerrain() {}

			void Construct   (const FArguments& InArgs);
			void Init        (const TArray<FVector2D>* InVertices, const Defcon::I2DCoordMapper* InMapper);
			void SetSize     (const FVector2D& InSize);


			virtual int32 OnPaint
			(
				const FPaintArgs&          Args,
				const FGeometry&           AllottedGeometry,
				const FSlateRect&          MyCullingRect,
				FSlateWindowElementList&   OutDrawElements,
				int32                      LayerId,
				const FWidgetStyle&        InWidgetStyle,
				bool                       bParentEnabled
			) const override;

			virtual FVector2D ComputeDesiredSize(float) const override;


		protected:

			const TArray<FVector2D>*       Vertices = nullptr;
			const Defcon::I2DCoordMapper*  Mapper   = nullptr;
			FVector2D                      Size;

			virtual bool ComputeVolatility() const override { return true; }
};
