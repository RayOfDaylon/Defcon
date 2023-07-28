// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPostwaveViewBase.h"
#include "DaylonUtils.h"
#include "Globals/_sound.h"
#include "DefconGameInstance.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "Common/util_color.h"
#include "DefconUtils.h"
#include "DefconLogging.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"



// -----------------------------------------------------------------------------------------

const float PVWarpSoundLength  = 2.8f;
const float PVStartingTextX    = 3000.0f;
const float PVTimeToShowStars  = PVWarpSoundLength + 0.2f;
const float PVTimeToCenterText = 1.0f;
const float PVTimeToRead       = 2.0f;
const float PVTimeToTransition = PVTimeToShowStars + PVTimeToCenterText + PVTimeToRead;
const float PVTimeForOneHuman  = 0.1f;


void UDefconPostwaveViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.

	Super::NativeOnInitialized();
}


void UDefconPostwaveViewBase::OnActivate()
{
	Super::OnActivate();

	// Start the title texts way offstage.
	// Assume they are at center horizontally.

	// todo: involve viewportsize.X
	PositionTexts(-PVStartingTextX, PVStartingTextX);

	Daylon::Hide(HumansRemainingGraph);

	bPlayedWaveEndSound = false;
}


void UDefconPostwaveViewBase::OnFinishActivating()
{
	Super::OnFinishActivating();

	Daylon::Show(Starfield);
	
	Starfield->Count         = 6000;
	Starfield->Speed         =   20.0f;
	Starfield->RotationSpeed =    7.0f;
	Starfield->Age           =    0.0f;
	Starfield->FadeStartsAt  = PVTimeToShowStars * 0.5f;
	Starfield->FadeEndsAt    = PVTimeToShowStars;

	Starfield->SynchronizeProperties();

	Starfield->SetColors(
	{
		C_LIGHTBLUE, C_GREEN, C_GREEN, C_LIGHTBLUE, C_CYAN, C_CYAN, C_CYAN,
		C_LIGHTERBLUE, C_LIGHTYELLOW, C_LIGHTYELLOW, C_LIGHTGREEN,
		C_LIGHTBLUE, C_CYAN, C_CYAN, C_LIGHTERBLUE, C_LIGHTBLUE
	});


	gpAudio->OutputSound(Defcon::EAudioTrack::snd_warp);

	// Start by collapsing the dead human images at the right edge of the box.
	// and hiding (NOT collapsing) all the other human images. 

	//auto GI = UDefconUtils::GetGameInstance(this);

	//if(GI != nullptr)
	{
		auto Humans = HumansRemainingGraph->GetAllChildren();

		const int32 NumHumansAlive = gDefconGameInstance->GetHumans().Count();
		const int32 NumHumansDead = 15 - NumHumansAlive;

		for(int32 Index = 0; Index < 15; Index++)
		{
			Humans[Index]->SetVisibility(Index < NumHumansAlive ? ESlateVisibility::Hidden : ESlateVisibility::Collapsed);
		}
	}
}


void UDefconPostwaveViewBase::PositionTexts(float XTitle, float XSubtitle)
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


void UDefconPostwaveViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if(bFirstTime)
	{
		bFirstTime = false;
	}

	Age += DeltaTime;

	if(Age < PVTimeToShowStars)
	{
		Starfield->Tick(DeltaTime);
		return;
	}

	
	Daylon::Hide(Starfield);

	//auto GI = UDefconUtils::GetGameInstance(this);

	if(!Subtitle->GetText().IsEmpty()) // current mission is already the next one, we need the state for the mission prior.
	{
		Daylon::Show(HumansRemainingGraph);

		if(!bPlayedWaveEndSound)
		{
			bPlayedWaveEndSound = true;
			gpAudio->OutputSound(Defcon::EAudioTrack::snd_wave_end);
		}

		// Populate the HumansRemainingGraph box, at one human per 1/8 second.
		const float HumansAge = Age - PVTimeToShowStars;

		const int32 NumHumansAlive = gDefconGameInstance->GetHumans().Count();

		if(HumansAge < NumHumansAlive * PVTimeForOneHuman)
		{
			const int32 IndexOfCurrentHuman = FMath::Min(NumHumansAlive - 1, ROUND(HumansAge / PVTimeForOneHuman));

			HumansRemainingGraph->GetChildAt(IndexOfCurrentHuman)->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}

	if(Age >= PVTimeToTransition)
	{
		StartMission();
		return;
	}

	const float T = FMath::Min(1.0f, Age / PVTimeToCenterText);

	const float X = FMath::Lerp(PVStartingTextX, 0.0f, T);

	PositionTexts(-X, X);

	// Pulse the subtitle.
	
	const float T2 = (float)fabs(sin(Age * PI * 2));
	const FSlateColor Color(FLinearColor::LerpUsingHSV(C_RED, C_YELLOW, T2));

	Subtitle->SetColorAndOpacity(Color);
}


int32 UDefconPostwaveViewBase::NativePaint
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
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}


void UDefconPostwaveViewBase::SetTexts(const FString& InTitle, const FString& InSubtitle)
{
	Title->SetText(FText::FromString(InTitle));
	Subtitle->SetText(FText::FromString(InSubtitle));
}


void UDefconPostwaveViewBase::SetTitle    (const FString& Str) { Title->SetText(FText::FromString(Str)); }
void UDefconPostwaveViewBase::SetSubtitle (const FString& Str) { Subtitle->SetText(FText::FromString(Str)); }


void UDefconPostwaveViewBase::StartMission()
{
	TransitionToArena(gDefconGameInstance->GetMission() != nullptr ? EDefconArena::Prewave : EDefconArena::GameOver);
}


void UDefconPostwaveViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MissionPicker);
}

void UDefconPostwaveViewBase::OnSkipPressed()
{
	StartMission();
}

