// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/UDaylonStarfield.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "UMG/Public/Components/GridPanel.h"
#include "DefconGameOverViewBase.generated.h"




/*
	The "game over" view shows a victory screen if any humans survived,
	otherwise a "defeat" screen.
*/
UCLASS()
class DEFCON_API UDefconGameOverViewBase : public UDefconViewBase
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

	virtual bool RequiresGameMatch   () const override { return true; }

	virtual void OnActivate          () override;
	virtual void OnFinishActivating  () override;
	virtual void OnEscPressed        () override;
	virtual void OnSkipPressed       () override;



	UFUNCTION(BlueprintCallable, Category=Defcon)
	void SetTexts(const FString& InTitle, const FString& InSubtitle); // todo: s/b FText


	void PositionTexts(float XTitle, float XSubtitle);


	// Text that enters stage left
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Title;

	// Text that enters stage right
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Subtitle;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UGridPanel* Stats;

	FName  StatItemShotsFired;
	FName  StatItemDoubleShotsFired;
	FName  StatItemSmartbombsDetonated;
	FName  StatItemLaserKills;
	FName  StatItemSmartbombKills;
	FName  StatItemFFIs;
	FName  StatItemHits;
	FName  StatItemCollisions;
	FName  StatItemDeaths;

	public:
	
	void SetTitle    (const FString& Str);
	void SetSubtitle (const FString& Str);
};
