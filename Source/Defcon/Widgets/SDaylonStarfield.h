// Copyright 2003-2023 Daylon Graphics Ltd.

#pragma once


// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "SlateCore/Public/Widgets/SLeafWidget.h"




struct DEFCON_API FDaylonStarParticle
{
	FVector      P;
	FLinearColor ColorFar;
};


// SDaylonStarfield - a Slate widget with a custom Paint event.
// Used to draw a bunch of dots representing stars.
// Used in the Defcon project to draw the hyperspacing transition between missions.
class DEFCON_API SDaylonStarfield : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDaylonStarfield)
			: 
			  _Size                (FVector2D(1920, 1080))
			, _Count               (1000)
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D,    Size)
			SLATE_ATTRIBUTE(int32,        Count)
			SLATE_ATTRIBUTE(FSlateBrush,  Brush)

			SLATE_END_ARGS()

			SDaylonStarfield() {}

			~SDaylonStarfield() {}

			void Construct(const FArguments& InArgs);

			void SetCount          (int32 InCount);
			void SetSize           (const FVector2D& InSize);
			void SetBrush          (const FSlateBrush& InBrush);
			void SetSpeed          (float Speed);
			void SetRotationSpeed  (float Speed);
			void SetAge            (float N);
			void SetFadeStartsAt   (float N);
			void SetFadeEndsAt     (float N);
			void SetOsStart        (float N);
			void SetOsEnd          (float N);
			void SetColors         (const TArray<FLinearColor>& Colors);

			bool Update                   (float DeltaTime);
			void Reset                    ();


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

			TArray<FDaylonStarParticle> Stars;
			FVector2D                   Size;

			virtual bool ComputeVolatility() const override { return true; }

		private:

			TArray<FLinearColor> Colors;

			FSlateBrush                 Brush;
			float                       Age          =     0.0f;
			float                       FadeStartsAt =     0.0f;
			float                       FadeEndsAt   = 30000.0f;
			float                       OsStart      =     1.0f;
			float                       OsEnd        =     0.0f;
			int32                       Count;
			float                       SpeedMul = 1.0f;
			float                       Angle = 0.0f;
			float                       SpeedRotMul = 0.0f;
			bool                        bFat = false;
};
