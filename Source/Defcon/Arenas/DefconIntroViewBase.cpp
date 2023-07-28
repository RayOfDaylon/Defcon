// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconIntroViewBase.h"
#include "DefconLogging.h"
#include "DaylonUtils.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


const float TITLE_START    = 2.0f; // Show the title forming after this many seconds from game launch.
const float TITLE_LIFESPAN = 8.0f;
const float MinTitleDistortion =  100.0f;
const float MaxTitleDistortion = 400.0f;

// -----------------------------------------------------------------------------------------

static void Subdivide(TArray<FDaylonLine>& Lines)
{
	TArray<FDaylonLine> NewLines;

	for(const auto& OldLine : Lines)
	{
		const auto Midpoint = (OldLine.End + OldLine.Start) / 2;

		auto Line = OldLine;
		Line.End = Midpoint;
		NewLines.Add(Line);

		Line = OldLine;
		Line.Start = Midpoint;
		NewLines.Add(Line);
	}

	Lines = NewLines;
}


void UDefconIntroViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.

	Super::NativeOnInitialized();


	FString Text; // todo:
	const FString Filespec = FPaths::ProjectContentDir() / TEXT("Textures") / TEXT("gamename.dat");

	if(!FFileHelper::LoadFileToString(Text, *Filespec))
	{
		UE_LOG(LogGame, Error, TEXT("Can't load Content/Textures/gamename.dat file"));
		return;
	}

	//DebugSavePath->SetVisibility(ESlateVisibility::Visible);
	//DebugSavePath->SetText(FText::FromString(Filespec));


	TArray<FString> TextLines;
	Text.ParseIntoArrayLines(TextLines);

	bool bLineDefined = false;
	FDaylonLine Line;
	FVector2D pt;
	FBox2d bbox;
	TArray<FDaylonLine> Lines;

	for(auto& TextLine : TextLines)
	{
		TextLine.TrimStartAndEndInline();

		if(TextLine.IsEmpty())
		{
			continue;
		}
		
		TArray<FString> Parts;
		TextLine.ParseIntoArrayWS(Parts);

		if(Parts.Num() < 3 || !Parts[0].IsNumeric() || !Parts[1].IsNumeric())
		{
			continue;
		}

		const auto Code = Parts[2].ToUpper()[0];

		if(Code != 'M' && Code != 'L')
		{
			continue;
		}

		pt.Set(FCString::Atof(*Parts[0]), FCString::Atof(*Parts[1]));


		switch(Code)
		{
			case 'M':
				bLineDefined = false;
				Line.Start = pt;
				break;

			case 'L':
				if(bLineDefined)
				{
					Line.Start = Lines[Lines.Num() - 1].End;
					Line.End = pt;
				}
				else
				{
					Line.End = pt;
					bLineDefined = true;
				}
				break;
		}

#define FBOX2D_UNION(bbox_, Line_)	\
	bbox_.Min.X = FMath::Min(bbox_.Min.X, FMath::Min(Line_.Start.X, Line_.End.X));	\
	bbox_.Max.X = FMath::Max(bbox_.Max.X, FMath::Max(Line_.Start.X, Line_.End.X));	\
	bbox_.Min.Y = FMath::Min(bbox_.Min.Y, FMath::Min(Line_.Start.Y, Line_.End.Y));	\
	bbox_.Max.Y = FMath::Max(bbox_.Max.Y, FMath::Max(Line_.Start.Y, Line_.End.Y));

		
		if(bLineDefined)
		{
			FBOX2D_UNION(bbox, Line);

			Lines.Add(Line);
#ifdef _DEBUG
#if 0
			char sz[100];
			MySprintf(sz, "Line read = (%.3f, %.3f) - (%.3f, %.3f)\n", 
					line.pts[0].x, line.pts[0].y,
					line.pts[1].x, line.pts[1].y);
			OutputDebugString(sz);
#endif
#endif
		}
	}
	// Now translate so that vertices are centered 
	// within the bbox, upside down, and reform the bbox.

	pt = bbox.GetCenter();

	const FVector2D targetSize(1.0f * 351.0f, 1.0f * 69.0f);

	const FVector2D fscale(targetSize.X / bbox.GetSize().X, -targetSize.Y / bbox.GetSize().Y); 

	bbox = FBox2d(FVector2D(0), FVector2D(0));

	int32 i;
	for(i = 0; i < Lines.Num(); i++)
	{
		Lines[i].Start -= pt;
		Lines[i].End -= pt;
		Lines[i].Start *= fscale;
		Lines[i].End *= fscale;

		FBOX2D_UNION(bbox, Lines[i]);
	}

	// Break up the lines into smaller ones.
#if 1
	Subdivide(Lines);
	Subdivide(Lines);
	Subdivide(Lines);
	Subdivide(Lines);
#endif
#if 1
	// Now randomize the drawing order.
	// This is needed if we're drawing more lines as time goes by.
	for(i = 0; i < Lines.Num(); i++)
	{
		const int32 j = FMath::RandRange(0, Lines.Num() - 1);
		Lines.Swap(i, j);
	}
#endif

	// Copy our lines to the dest part of TitleLines.
	// And create the src part too.
	for(const auto& Line2 : Lines)
	{
		FDaylonLerpedLine LerpedLine;

		auto src = Line2;

		src.Start.Normalize();
		src.End.Normalize();
		src.Start *= FMath::FRandRange(MinTitleDistortion, MaxTitleDistortion);
		src.End   *= FMath::FRandRange(MinTitleDistortion, MaxTitleDistortion);

		LerpedLine.Src = src;

		LerpedLine.Dst = Line2;
		TitleLines.Add(LerpedLine);
	}

	TitleWidget = SNew(SDaylonLerpedLines);

	FDaylonLerpedLinesParams Params;

	Params.Color           = FLinearColor::Red;
	Params.Lifetime        = 5.0f;
	Params.FinalOpacity    = 1.0f;
	Params.StartingOpacity = 0.0f;
	Params.LineThickness   = 3.0f;

	Params.LerpedLines = TitleLines;

	TitleWidget->Set(Params);
}


void UDefconIntroViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if(bFirstTime)
	{
		bFirstTime = false;

		// Wait till here to create and install the widget.
		auto CanvasSlotArgs = RootCanvas->GetCanvasWidget()->AddSlot();

		CanvasSlotArgs[TitleWidget.ToSharedRef()];
		CanvasSlotArgs.Anchors(FAnchors(0.5f)); // place perfectly center
		CanvasSlotArgs.AutoSize(true);
		CanvasSlotArgs.Alignment(FVector2D(0.5));
		CanvasSlotArgs.Offset(FMargin(0, 0, 0, 0));
		CanvasSlotArgs.ZOrder(-1);

		Daylon::Hide(TitleWidget.Get());
	}

	Age += DeltaTime;

	if(!bIsTitleWidgetInstalled && Age >= TITLE_START / 2)
	{
		bIsTitleWidgetInstalled = true;

	}

	if(Age >= TITLE_START)
	{
		Daylon::Show(TitleWidget.Get());

		const float T = FMath::Min(1.0f, (Age - TITLE_START) / TITLE_LIFESPAN);

		TitleWidget->SetScale((1.0f - T) * 2 + 0.99f);

		if(!TitleWidget->Update(DeltaTime))
		{
			// let widget linger on screen for now. Otherwise, we shrink to zero/black and transition to main menu
			//RootCanvas->GetCanvasWidget()->RemoveSlot(TitleWidget.ToSharedRef());
			//TitleWidget.Reset();
		}
	}
}


int32 UDefconIntroViewBase::NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const
{
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}


void UDefconIntroViewBase::OnEnterPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
