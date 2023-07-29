// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/UDaylonStarfield.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "DefconPostwaveViewBase.generated.h"





UCLASS()
class DEFCON_API UDefconPostwaveViewBase : public UDefconViewBase
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


	virtual void OnActivate          () override;
	virtual void OnFinishActivating  () override;
	virtual void OnEscPressed        () override;
	virtual void OnSkipPressed       () override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDaylonStarfield* Starfield;


	UFUNCTION(BlueprintCallable, Category=Defcon)
	void SetTexts(const FString& InTitle, const FString& InSubtitle); // todo: s/b FText

	void StartMission();

	void PositionTexts(float XTitle, float XSubtitle);


	// Text that enters stage left
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Title;

	// Text that enters stage right
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Subtitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* HumansRemainingGraph;


	public:
	
	void SetTitle    (const FString& Str);
	void SetSubtitle (const FString& Str);

	bool bPlayedWaveEndSound = false;
};
