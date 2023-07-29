// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/SLerpedLines.h"
#include "DefconIntroViewBase.generated.h"


UCLASS()
class DEFCON_API UDefconIntroViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	/*virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;*/


	virtual void OnEnterPressed() override;



	TSharedPtr<SDaylonLerpedLines> TitleWidget;

	TArray<FDaylonLerpedLine> TitleLines;

	bool bFirstTime                = true;
	//bool bIsTitleWidgetInstalled = false;
};
