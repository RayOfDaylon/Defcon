// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconHelpViewBase.h"
#include "DefconGameInstance.h"
#include "DefconLogging.h"
#include "Common/util_color.h"


#if 0
void UDefconHelpViewBase::NativeOnInitialized()
{
	// Read the title vector file and set up the lerped lines widget.
	LOG_UWIDGET_FUNCTION

	Super::NativeOnInitialized();
}


void UDefconHelpViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();
}


int32 UDefconHelpViewBase::NativePaint
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


void UDefconHelpViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);


	// Oscillate help text between orange and yellow.

	if(!IsValid(KeyboardControlsList))
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

	for(int32 Index = 0; Index < KeyboardControlsList->GetChildrenCount(); Index++)
	{
		auto TextBlock = Cast<UTextBlock>(KeyboardControlsList->GetChildAt(Index));

		if(TextBlock == nullptr)
		{
			continue;
		}

		TextBlock->SetColorAndOpacity(SlateColor);
	}
}


void UDefconHelpViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}
