// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "SLerpedLines.h"

DEFINE_LOG_CATEGORY(LogDaylonLerpedLines);


void SDaylonLerpedLines::Construct(const FArguments& InArgs)
{
	Color           = InArgs._Color.Get();
	LineThickness   = InArgs._LineThickness.Get();
	StartingOpacity = InArgs._StartingOpacity.Get();
	FinalOpacity    = InArgs._FinalOpacity.Get();
	Lifetime        = InArgs._Lifetime.Get();
}


void SDaylonLerpedLines::Set(const FDaylonLerpedLinesParams& Params)
{
	LerpedLines     = Params.LerpedLines;
	Color           = Params.Color;
	LineThickness   = Params.LineThickness;
	StartingOpacity = Params.StartingOpacity;
	FinalOpacity    = Params.FinalOpacity;
	Lifetime        = Params.Lifetime;

	Reset();
}


void SDaylonLerpedLines::Reset()
{
	Age = 0.0f;
}


bool SDaylonLerpedLines::Update(float DeltaTime)
{
	Age += DeltaTime;

	return (Age < Lifetime);
}


FVector2D SDaylonLerpedLines::ComputeDesiredSize(float) const
{
	return Size;
}


int32 SDaylonLerpedLines::OnPaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	// UE_LOG(LogDaylonLerpedLines, Warning, TEXT("%S: Geom: %d, %d"), __FUNCTION__, (int32)AllottedGeometry.AbsolutePosition.X, (int32)AllottedGeometry.AbsolutePosition.Y);

	const float T = FMath::Min(1.0f, Age / Lifetime);

	float CurrentOpacity = FMath::Lerp(StartingOpacity, FinalOpacity, T);
	//CurrentOpacity = FMath::Min(1.0f, CurrentOpacity);

	auto TheColor = Color;

	TheColor.A = CurrentOpacity;

	// Render more and more of the lines as time goes by
	int32 LinesToDraw = FMath::Lerp(1, LerpedLines.Num(), T);

	for(int32 Index = 0; Index < LinesToDraw; Index++/*const auto& LerpedLine : LerpedLines*/)
	{
		const auto& LerpedLine = LerpedLines[Index];
		FDaylonLine Line;
		//LerpedLine.Get(T, Line);
		LerpedLine.GetRelative(T, Line);

		TArray<FVector2f> Points;

		Points.Add(UE::Slate::CastToVector2f(Line.Start * Scale));
		Points.Add(UE::Slate::CastToVector2f(Line.End   * Scale));

		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, TheColor, true, LineThickness);
	}

	return LayerId;
}
