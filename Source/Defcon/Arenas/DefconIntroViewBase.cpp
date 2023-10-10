// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconIntroViewBase.h"
#include "DefconLogging.h"
#include "DaylonUtils.h"
#include "Common/util_math.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


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

#if 1
	Text =
		"22.9367 -2.3782 m\n"
		"39.2491 -2.3782 L\n"
		"39.2491 -14.7618 L\n"
		"22.9367 -14.7618 L\n"
		"22.9367 -2.3782 L\n"
		"18.0617 -17.0657 m\n"
		"39.3447 -17.0657 L\n"
		"44.1241 -14.7618 L\n"
		"44.1241 -2.3782 L\n"
		"39.3447 -0.0743 L\n"
		"18.0617 -0.0743 L\n"
		"18.0617 -17.0657 L\n"
		"49.9821 -17.0657 m\n"
		"49.9821 -0.0743 L\n"
		"73.8169 -0.0743 L\n"
		"69.3049 -2.3782 L\n"
		"54.8571 -2.3782 L\n"
		"54.8571 -7.1993 L\n"
		"71.5609 -7.1993 L\n"
		"66.953 -9.5032 L\n"
		"54.8571 -9.5032 L\n"
		"54.8571 -14.7618 L\n"
		"73.8169 -14.7618 L\n"
		"69.3049 -17.0657 L\n"
		"49.9821 -17.0657 L\n"
		"77.486 -0.0743 m\n"
		"101.7528 -0.0743 L\n"
		"97.2408 -2.3782 L\n"
		"82.361 -2.3782 L\n"
		"82.361 -7.4493 L\n"
		"99.1608 -7.4493 L\n"
		"94.4569 -9.7532 L\n"
		"82.361 -9.7532 L\n"
		"82.361 -17.07 L\n"
		"77.486 -17.07 L\n"
		"77.486 -0.0743 L\n"
		"103.2146 -17.0657 m\n"
		"103.2146 -0.0743 L\n"
		"127.0493 -0.0743 L\n"
		"122.5374 -2.3782 L\n"
		"108.0895 -2.3782 L\n"
		"108.0895 -7.1993 L\n"
		"124.7933 -7.1993 L\n"
		"120.1854 -9.5032 L\n"
		"108.0895 -9.5032 L\n"
		"108.0895 -14.7618 L\n"
		"127.0493 -14.7618 L\n"
		"122.5374 -17.0657 L\n"
		"103.2146 -17.0657 L\n"
		"130.7243 -0.8162 m\n"
		"135.3376 0.2425 L\n"
		"151.7533 -12.8383 L\n"
		"151.8492 -12.8383 L\n"
		"151.8492 -0.8162 L\n"
		"156.4625 0.2425 L\n"
		"156.4625 -17.07 L\n"
		"151.8012 -17.07 L\n"
		"135.4333 -3.942 L\n"
		"135.3376 -3.942 L\n"
		"135.3376 -17.07 L\n"
		"130.7243 -17.07 L\n"
		"130.7243 -0.8162 L\n"
		"162.7639 -2.3782 m\n"
		"167.4476 -0.0743 L\n"
		"191.3982 -0.0743 L\n"
		"186.6056 -2.3782 L\n"
		"167.6389 -2.3782 L\n"
		"167.6389 -7.1993 L\n"
		"183.6425 -7.1993 L\n"
		"188.3263 -9.6468 L\n"
		"188.3263 -14.6184 L\n"
		"183.6425 -17.0657 L\n"
		"160.0759 -17.0657 L\n"
		"164.58 -14.7618 L\n"
		"183.4513 -14.7618 L\n"
		"183.4513 -9.5032 L\n"
		"167.1609 -9.5032 L\n"
		"162.7639 -7.3433 L\n"
		"162.7639 -2.3782 L\n"
		"194.5593 -17.0657 m\n"
		"194.5593 -0.0743 L\n"
		"218.394 -0.0743 L\n"
		"213.8821 -2.3782 L\n"
		"199.4343 -2.3782 L\n"
		"199.4343 -7.1993 L\n"
		"216.1381 -7.1993 L\n"
		"211.5301 -9.5032 L\n"
		"199.4343 -9.5032 L\n"
		"199.4343 -14.7618 L\n"
		"218.394 -14.7618 L\n"
		"213.8821 -17.0657 L\n"
		"194.5593 -17.0657 L\n"
		"0.0601 -31.3782 m\n"
		"4.8395 -29.0743 L\n"
		"21.6389 -29.0743 L\n"
		"26.1509 -31.3782 L\n"
		"4.9351 -31.3782 L\n"
		"4.9351 -43.7618 L\n"
		"26.4389 -43.7618 L\n"
		"21.9269 -46.0657 L\n"
		"4.8395 -46.0657 L\n"
		"0.0601 -43.7618 L\n"
		"0.0601 -31.3782 L\n"
		"34.2152 -31.3782 m\n"
		"50.8401 -31.3782 L\n"
		"50.8401 -43.7618 L\n"
		"34.2152 -43.7618 L\n"
		"34.2152 -31.3782 L\n"
		"29.3402 -43.7618 m\n"
		"33.9283 -46.0657 L\n"
		"51.1268 -46.0657 L\n"
		"55.7151 -43.7618 L\n"
		"55.7151 -31.3782 L\n"
		"51.1268 -29.0743 L\n"
		"33.9283 -29.0743 L\n"
		"29.3402 -31.3782 L\n"
		"29.3402 -43.7618 L\n"
		"61.5789 -29.8162 m\n"
		"66.1922 -28.7575 L\n"
		"82.6079 -41.8383 L\n"
		"82.7038 -41.8383 L\n"
		"82.7038 -29.8162 L\n"
		"87.3171 -28.7575 L\n"
		"87.3171 -46.07 L\n"
		"82.6558 -46.07 L\n"
		"66.2879 -32.942 L\n"
		"66.1922 -32.942 L\n"
		"66.1922 -46.07 L\n"
		"61.5789 -46.07 L\n"
		"61.5789 -29.8162 L\n"
		"98.3685 -31.3782 m\n"
		"114.6809 -31.3782 L\n"
		"114.6809 -43.7618 L\n"
		"98.3685 -43.7618 L\n"
		"98.3685 -31.3782 L\n"
		"93.4935 -46.0657 m\n"
		"114.7765 -46.0657 L\n"
		"119.5559 -43.7618 L\n"
		"119.5559 -31.3782 L\n"
		"114.7765 -29.0743 L\n"
		"93.4935 -29.0743 L\n"
		"93.4935 -46.0657 L\n"
		"126.1014 -29.07 m\n"
		"130.9764 -29.07 L\n"
		"130.9764 -46.07 L\n"
		"126.1014 -46.07 L\n"
		"126.1014 -29.07 L\n"
		"145.1734 -31.3782 m\n"
		"133.3175 -31.3782 L\n"
		"138.0215 -29.0743 L\n"
		"161.9042 -29.0743 L\n"
		"157.2003 -31.3782 L\n"
		"150.0484 -31.3782 L\n"
		"150.0484 -46.07 L\n"
		"145.1734 -46.07 L\n"
		"145.1734 -31.3782 L\n"
		"164.2144 -29.07 m\n"
		"169.0894 -29.07 L\n"
		"169.0894 -46.07 L\n"
		"164.2144 -46.07 L\n"
		"164.2144 -29.07 L\n"
		"180.4739 -31.3782 m\n"
		"197.0989 -31.3782 L\n"
		"197.0989 -43.7618 L\n"
		"180.4739 -43.7618 L\n"
		"180.4739 -31.3782 L\n"
		"175.599 -43.7618 m\n"
		"180.1871 -46.0657 L\n"
		"197.3856 -46.0657 L\n"
		"201.9738 -43.7618 L\n"
		"201.9738 -31.3782 L\n"
		"197.3856 -29.0743 L\n"
		"180.1871 -29.0743 L\n"
		"175.599 -31.3782 L\n"
		"175.599 -43.7618 L\n"
		"207.8377 -29.8162 m\n"
		"212.451 -28.7575 L\n"
		"228.8667 -41.8383 L\n"
		"228.9626 -41.8383 L\n"
		"228.9626 -29.8162 L\n"
		"233.5759 -28.7575 L\n"
		"233.5759 -46.07 L\n"
		"228.9146 -46.07 L\n"
		"212.5467 -32.942 L\n"
		"212.451 -32.942 L\n"
		"212.451 -46.07 L\n"
		"207.8377 -46.07 L\n"
		"207.8377 -29.8162 L\n"
	;
#else
	const FString Filespec = FPaths::ProjectContentDir() / TEXT("Textures") / TEXT("gamename.dat");

	if(!FFileHelper::LoadFileToString(Text, *Filespec))
	{
		UE_LOG(LogGame, Error, TEXT("Can't load Content/Textures/gamename.dat file"));
		return;
	}
#endif
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


void UDefconIntroViewBase::OnEscPressed()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
