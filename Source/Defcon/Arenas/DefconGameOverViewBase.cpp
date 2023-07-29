// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconGameOverViewBase.h"
#include "DaylonUtils.h"
#include "Globals/_sound.h"
#include "DefconGameInstance.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "Common/util_color.h"
#include "DefconUtils.h"
#include "DefconLogging.h"

#include "UMG/Public/Components/CanvasPanelSlot.h"



// -----------------------------------------------------------------------------------------

const float StartingTextX    = 3000.0f;
const float TimeToCenterText = 1.0f;
const float TimeToRead       = 2.0f;
const float TimeToTransition = TimeToCenterText + TimeToRead;
const float TimeForOneHuman  = 0.1f;


void UDefconGameOverViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.
	LOG_UWIDGET_FUNCTION

	Super::NativeOnInitialized();
}


void UDefconGameOverViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();

	// Start the title texts way offstage.
	// Assume they are at center horizontally.

	auto Mission = gDefconGameInstance->GetMission();

	if(Mission == nullptr && gDefconGameInstance->GetHumans().Count() > 0)
	{
		SetTitle(TEXT("VICTORY"));
		SetSubtitle(TEXT("You defended the humans"));
	}
	else
	{
		SetTitle(TEXT("DEFEAT"));
		SetSubtitle(TEXT("All humans were killed or abducted"));
	}

	// todo: involve viewportsize.X
	PositionTexts(-StartingTextX, StartingTextX);
}


void UDefconGameOverViewBase::OnFinishActivating()
{
	LOG_UWIDGET_FUNCTION
	Super::OnFinishActivating();


	// Start by collapsing the dead human images at the right edge of the box.
	// and hiding (NOT collapsing) all the other human images. 
	/*
	auto GI = UDefconUtils::GetGameInstance(this);

	if(GI != nullptr)
	{
	}*/
}


void UDefconGameOverViewBase::PositionTexts(float XTitle, float XSubtitle)
{
	auto CanvasSlot = Cast<UCanvasPanelSlot>(Title->Slot);

	auto P = CanvasSlot->GetPosition();
	P.X = XTitle;
	CanvasSlot->SetPosition(P);

	CanvasSlot = Cast<UCanvasPanelSlot>(Subtitle->Slot);

	P = CanvasSlot->GetPosition();
	P.X = XSubtitle; 
	CanvasSlot->SetPosition(P);
}


void UDefconGameOverViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);
	

	//auto GI = UDefconUtils::GetGameInstance(this);

	if(!Subtitle->GetText().IsEmpty()) // current mission is already the next one, we need the state for the mission prior.
	{
	}

	if(Age >= TimeToTransition)
	{
		return;
	}

	const float T = FMath::Min(1.0f, Age / TimeToCenterText);

	const float X = FMath::Lerp(StartingTextX, 0.0f, T);

	PositionTexts(-X, X);

	// Pulse the subtitle.
	
	const float T2 = (float)fabs(sin(Age * PI * 2));
	const FSlateColor Color(FLinearColor::LerpUsingHSV(C_RED, C_YELLOW, T2));

	Subtitle->SetColorAndOpacity(Color);
}


#if 0
int32 UDefconGameOverViewBase::NativePaint
(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	LOG_UWIDGET_FUNCTION
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}
#endif


void UDefconGameOverViewBase::SetTexts(const FString& InTitle, const FString& InSubtitle)
{
	Title->SetText(FText::FromString(InTitle));
	Subtitle->SetText(FText::FromString(InSubtitle));
}


void UDefconGameOverViewBase::SetTitle    (const FString& Str) { Title->SetText(FText::FromString(Str)); }
void UDefconGameOverViewBase::SetSubtitle (const FString& Str) { Subtitle->SetText(FText::FromString(Str)); }


void UDefconGameOverViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}

void UDefconGameOverViewBase::OnSkipPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}

