// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayStatsWidgetBase.h"



void UDefconPlayStatsWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}


void UDefconPlayStatsWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
}


int32 UDefconPlayStatsWidgetBase::NativePaint
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
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}


void UDefconPlayStatsWidgetBase::UpdateShieldReadout(float Amount)
{
	check(Amount >= 0.0f && Amount <= 1.0f);

	ShieldReadout->SetPercent(Amount);
}


void UDefconPlayStatsWidgetBase::UpdateSmartbombReadout(int32 Amount)
{
	check(Amount >= 0);

	const FString Str = FString::Printf(TEXT("%d"), Amount);

	SmartbombReadout->SetText(FText::FromString(Str));
}