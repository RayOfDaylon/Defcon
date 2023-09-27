// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconPlayWidgetBase.h"
#include "DefconHumansInfoBase.h"
#include "Widgets/UDaylonProgressBar.h"
#include "UMG/Public/Components/TextBlock.h"
//#include "UMG/Public/Components/ProgressBar.h"
#include "DefconPlayStatsWidgetBase.generated.h"


/*
	This widget shows player stats like shield level, smartbomb count, etc.
	It normally sits above the main widget and to the left of the radar widget.
*/
UCLASS()
class DEFCON_API UDefconPlayStatsWidgetBase : public UDefconPlayWidgetBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs&          Args,
		const FGeometry&           AllottedGeometry,
		const FSlateRect&          MyCullingRect,
		FSlateWindowElementList&   OutDrawElements,
		int32                      LayerId,
		const FWidgetStyle&        InWidgetStyle,
		bool                       bParentEnabled) const override;


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UDaylonProgressBar> ShieldReadout;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> ShieldLabel;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> HumansLabel;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UDaylonProgressBar> DoubleGunsReadout;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> DoubleGunsLabel;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UDaylonProgressBar> InvincibilityReadout;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> InvincibilityLabel;


	TArray<FLinearColor>    ShieldGradient;

	bool FlashHumansLabel = false;
};


