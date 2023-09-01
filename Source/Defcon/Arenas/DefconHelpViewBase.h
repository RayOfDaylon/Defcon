// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/GridPanel.h"
#include "DefconHelpViewBase.generated.h"





UCLASS()
class DEFCON_API UDefconHelpViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

#if 0
	virtual void NativeOnInitialized() override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;


	virtual void OnActivate   () override;
#endif
	virtual void OnEscPressed   () override;
	virtual void OnSkipPressed  () override;
	virtual void OnEnterPressed () override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UGridPanel* KeyboardControlsList;
};
