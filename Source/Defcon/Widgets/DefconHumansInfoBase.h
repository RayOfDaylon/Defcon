// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

//#include "DefconPlayWidgetBase.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "DefconHumansInfoBase.generated.h"


/*
	This widget shows a simple list of icons representing each human.
*/
UCLASS()
class DEFCON_API UDefconHumansInfoBase : public UUserWidget
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

/*	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs&          Args,
		const FGeometry&           AllottedGeometry,
		const FSlateRect&          MyCullingRect,
		FSlateWindowElementList&   OutDrawElements,
		int32                      LayerId,
		const FWidgetStyle&        InWidgetStyle,
		bool                       bParentEnabled) const override;
*/
/*
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UWidget> ?;
*/

	public:

	void UpdateReadout          (const TArray<bool>& AbductionStates);
};


