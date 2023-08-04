// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconIntroViewBase.h"
#include "DefconLogging.h"
#include "DaylonUtils.h"
#include "Common/util_math.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


constexpr float TITLE_START        =   2.0f; // Show the title forming after this many seconds from game launch.
constexpr float TITLE_LIFESPAN     =   8.0f;
constexpr float MinTitleDistortion = 100.0f;
constexpr float MaxTitleDistortion = 400.0f;

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
		LOG_UWIDGET_FUNCTION

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
	FVector2D Pt;
	FBox2d Bbox;
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

		Pt.Set(FCString::Atof(*Parts[0]), FCString::Atof(*Parts[1]));


		switch(Code)
		{
			case 'M':
				bLineDefined = false;
				Line.Start = Pt;
				break;

			case 'L':
				if(bLineDefined)
				{
					Line.Start = Lines[Lines.Num() - 1].End;
					Line.End = Pt;
				}
				else
				{
					Line.End = Pt;
					bLineDefined = true;
				}
				break;
		}

#define FBOX2D_UNION(Bbox_, Line_)	\
	Bbox_.Min.X = FMath::Min(Bbox_.Min.X, FMath::Min(Line_.Start.X, Line_.End.X));	\
	Bbox_.Max.X = FMath::Max(Bbox_.Max.X, FMath::Max(Line_.Start.X, Line_.End.X));	\
	Bbox_.Min.Y = FMath::Min(Bbox_.Min.Y, FMath::Min(Line_.Start.Y, Line_.End.Y));	\
	Bbox_.Max.Y = FMath::Max(Bbox_.Max.Y, FMath::Max(Line_.Start.Y, Line_.End.Y));

		
		if(bLineDefined)
		{
			FBOX2D_UNION(Bbox, Line);

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

	Pt = Bbox.GetCenter();

	const FVector2D targetSize(1.0f * 351.0f, 1.0f * 69.0f);

	const FVector2D fscale(targetSize.X / Bbox.GetSize().X, -targetSize.Y / Bbox.GetSize().Y); 

	Bbox = FBox2d(FVector2D(0), FVector2D(0));

	for(int32 i = 0; i < Lines.Num(); i++)
	{
		Lines[i].Start -= Pt;
		Lines[i].End -= Pt;
		Lines[i].Start *= fscale;
		Lines[i].End *= fscale;

		FBOX2D_UNION(Bbox, Lines[i]);
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
	for(int32 i = 0; i < Lines.Num(); i++)
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

		auto Src = Line2;

		Src.Start.Normalize();
		Src.End.Normalize();
		Src.Start *= FMath::FRandRange(MinTitleDistortion, MaxTitleDistortion);
		Src.End   *= FMath::FRandRange(MinTitleDistortion, MaxTitleDistortion);

		LerpedLine.Src = Src;

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
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);

	if(bFirstTime)
	{
		bFirstTime = false;

		// Wait till here to create and install the Slate widget.
		auto CanvasSlotArgs = RootCanvas->GetCanvasWidget()->AddSlot();

		CanvasSlotArgs[TitleWidget.ToSharedRef()];
		CanvasSlotArgs.Anchors(FAnchors(0.5f)); // place perfectly center
		CanvasSlotArgs.AutoSize(true);
		CanvasSlotArgs.Alignment(FVector2D(0.5));
		CanvasSlotArgs.Offset(FMargin(0, 0, 0, 0));
		CanvasSlotArgs.ZOrder(-1);

		Daylon::Hide(TitleWidget.Get());
	}


	if(Age >= TITLE_START)
	{
		Daylon::Show(TitleWidget.Get());

		const float T = FMath::Min(1.0f, (Age - TITLE_START) / TITLE_LIFESPAN);

		TitleWidget->SetScale((1.0f - T) * 2 + 0.99f);

		if(!TitleWidget->Update(DeltaTime))
		{
			// Let widget linger on screen for now. Otherwise, we shrink to zero/black and transition to main menu
			//RootCanvas->GetCanvasWidget()->RemoveSlot(TitleWidget.ToSharedRef());
			//TitleWidget.Reset();
		}

		// As the title starts collapsing, outro the company name.

		const float CompanyNameOs = FMath::Max(0.0f, 1.0f - NORM_(Age, TITLE_START, TITLE_START + TITLE_LIFESPAN / 2));

		CompanyName->SetRenderOpacity(CompanyNameOs);
	}
}


void UDefconIntroViewBase::OnEnterPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
