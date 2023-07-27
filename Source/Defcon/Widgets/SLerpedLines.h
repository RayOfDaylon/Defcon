// Copyright 2023 Daylon Graphics Ltd.

#pragma once


// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "SlateCore/Public/Widgets/SLeafWidget.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDaylonLerpedLines, Log, All);


// SDaylonLerpedLines - a Slate widget with a custom Paint event.
// Used to draw e.g. a bunch of lines that coalesce into something recognizable.
// Used in the Defcon project to draw the introduction game title graphic.

struct DEFCON_API FDaylonLine
{
	FVector2D Start, End;
};


struct DEFCON_API FDaylonLerpedLine
{
	// A single line particle.
	// The widget maintains an array of these.

	// Endpoints of line as it appears at the start and end.
	FDaylonLine   Src, Dst;

	void Get(float T, FDaylonLine& Line) const
	{
		Line.Start.Set(FMath::Lerp(Src.Start.X, Dst.Start.X, T), FMath::Lerp(Src.Start.Y, Dst.Start.Y, T));
		Line.End.Set  (FMath::Lerp(Src.End.X,   Dst.End.X, T),   FMath::Lerp(Src.End.Y,   Dst.End.Y,   T));
	}

	void GetRelative(float T, FDaylonLine& Line) const
	{
		auto Offset = Src;

		// As m_fContent reaches 1.0, v tends towards origin.
 		// This shrinks the offset revealing the proper title shape.
		Offset.Start.X = FMath::Lerp(Offset.Start.X, 0.0, T);
		Offset.Start.Y = FMath::Lerp(Offset.Start.Y, 0.0, T);
		Offset.End.X   = FMath::Lerp(Offset.End.X,   0.0, T);
		Offset.End.Y   = FMath::Lerp(Offset.End.Y,   0.0, T);

		Line.Start = Dst.Start + Offset.Start;
		Line.End   = Dst.End + Offset.End;
	}
};


struct DEFCON_API FDaylonLerpedLinesParams
{
	TArray<FDaylonLerpedLine> LerpedLines;

	FLinearColor Color;
	float        LineThickness;
	float        StartingOpacity;
	float        FinalOpacity;
	float        Lifetime;
};


class DEFCON_API SDaylonLerpedLines : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDaylonLerpedLines)
			: 
			  _Size                (FVector2D(4))
			, _Color               (FLinearColor::White)
			, _Lifetime            (5.0f)
			, _LineThickness       (1.0f)
			, _StartingOpacity     (1.0f)
			, _FinalOpacity        (1.0f)
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D,    Size)
			SLATE_ATTRIBUTE(FLinearColor, Color)
			SLATE_ATTRIBUTE(float,        Lifetime)
			SLATE_ATTRIBUTE(float,        LineThickness)
			SLATE_ATTRIBUTE(float,        StartingOpacity)
			SLATE_ATTRIBUTE(float,        FinalOpacity)

			SLATE_END_ARGS()

			SDaylonLerpedLines() {}

			~SDaylonLerpedLines() {}

			void Construct(const FArguments& InArgs);

			void Set                      (const FDaylonLerpedLinesParams& Params);
			void SetScale                 (float InScale) { Scale = InScale; }
			//void SetOpacity               
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

			TArray<FDaylonLerpedLine>   LerpedLines;
			FVector2D                   Size = FVector2D(4);
			FLinearColor                Color;
			float                       Lifetime;
			float                       LineThickness;
			float                       StartingOpacity;
			float                       FinalOpacity;

			virtual bool ComputeVolatility() const override { return true; }

		private:

			float                       Age;
			float                       Scale = 1.0f;
};
