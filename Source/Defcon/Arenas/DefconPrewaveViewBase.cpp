// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPrewaveViewBase.h"
#include "DefconGameInstance.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "Common/util_color.h"
#include "DefconLogging.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"



// -----------------------------------------------------------------------------------------

const float PostWaveStartingTextX    = 3000.0f;
const float PostWaveTimeToCenterText = 1.0f;
const float PostWaveTimeToRead       = 2.0f;
const float PostWaveTimeToTransition = PostWaveTimeToCenterText + PostWaveTimeToRead;


void UDefconPrewaveViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.

	LOG_UWIDGET_FUNCTION
	Super::NativeOnInitialized();
}


void UDefconPrewaveViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();

	// Start the title texts way offstage.
	// Assume they are at center horizontally.

	// todo: involve viewportsize.X
	PositionTexts(-PostWaveStartingTextX, PostWaveStartingTextX);

	// Set the mission name
	//auto GameInstance = gDefconGameInstance;

	auto Mission = Defcon::GGameMatch->GetMission();// Defcon::CMissionFactory::Make(Defcon::GGameMatch->GetMissionID());

	check(Mission != nullptr);
	//{
		//UE_LOG(LogGame, Error, TEXT("%S: invalid missionID %d"), __FUNCTION__, (int32)GDefconGameInstance->MissionID);
		//return;
	//}

	Title->SetText(FText::FromString(Mission->GetName()));

	//delete pMission;
}


void UDefconPrewaveViewBase::PositionTexts(float XTitle, float XSubtitle)
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


void UDefconPrewaveViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);


	if(Age >= PostWaveTimeToTransition)
	{
		StartMission();
		return;
	}

	const float T = FMath::Min(1.0f, Age / PostWaveTimeToCenterText);

	const float X = FMath::Lerp(PostWaveStartingTextX, 0.0f, T);

	PositionTexts(-X, X);

	// Pulse the subtitle.
	
	const float T2 = (float)fabs(sin(Age * PI * 2));
	const FSlateColor Color(FLinearColor::LerpUsingHSV(C_RED, C_YELLOW, T2));

	Subtitle->SetColorAndOpacity(Color);
}


#if 0
int32 UDefconPrewaveViewBase::NativePaint
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


void UDefconPrewaveViewBase::SetTexts(const FString& InTitle, const FString& InSubtitle)
{
	Title->SetText(FText::FromString(InTitle));
	Subtitle->SetText(FText::FromString(InSubtitle));
}


void UDefconPrewaveViewBase::StartMission()
{
	TransitionToArena(EDefconArena::Play);
}


void UDefconPrewaveViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MissionPicker);
}

void UDefconPrewaveViewBase::OnSkipPressed()
{
	StartMission();
}

