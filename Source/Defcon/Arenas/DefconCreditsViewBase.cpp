// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconCreditsViewBase.h"
#include "DefconGameInstance.h"
#include "DefconLogging.h"
#include "Common/util_color.h"
//#include "UMG/Public/Components/CanvasPanelSlot.h"



void UDefconCreditsViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.

	Super::NativeOnInitialized();
}


void UDefconCreditsViewBase::OnActivate()
{
	Super::OnActivate();
}




void UDefconCreditsViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if(bFirstTime)
	{
		bFirstTime = false;
	}

	Age += DeltaTime;

	

	// Oscillate help text between orange and yellow.

	if(!IsValid(Content))
	{
		return;
	}

	const float T = fabs(sin(Age * HALF_PI));

	FLinearColor Color;
	// HSV looks too fast, use RGB lerp.
	//Color.LerpUsingHSV(C_ORANGE, C_YELLOW, T);

	Color.R = FMath::Lerp(C_RED.R, C_YELLOW.R, T);
	Color.G = FMath::Lerp(C_RED.G, C_YELLOW.G, T);
	Color.B = FMath::Lerp(C_RED.B, C_YELLOW.B, T);
	Color.A = 1.0f;

	FSlateColor SlateColor(Color);

	for(int32 Index = 0; Index < Content->GetChildrenCount(); Index++)
	{
		auto TextBlock = Cast<UTextBlock>(Content->GetChildAt(Index));

		if(TextBlock == nullptr)
		{
			continue;
		}

		TextBlock->SetColorAndOpacity(SlateColor);
	}
}


int32 UDefconCreditsViewBase::NativePaint
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


void UDefconCreditsViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}
