// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/DefconUserWidgetBase.h"
#include "UMG/Public/Components/UniformGridPanel.h"
#include "UMG/Public/Components/TextBlock.h"
#include "DefconMissionPickerViewBase.generated.h"


class UBorder;


struct FMissionInfo
{
	public:

		FString Name; // todo: should be FText
		FString Desc;
};



UCLASS()
class DEFCON_API UDefconMissionPickerViewBase : public UDefconViewBase
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

	virtual void OnActivate     () override;
	virtual void OnEscPressed   () override;
	virtual void OnEnterPressed () override;
	virtual void OnSkipPressed  () override;

	virtual void OnNavEvent     (ENavigationKey Key) override;


	UFUNCTION(BlueprintCallable, Category=Defcon)
	void OnChooseMission();


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* MissionsGrid;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MissionName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MissionDesc;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidget* StartMissionExpander;

	// The type of widget to use to populate the mission picker grid with.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UDefconUserWidgetBase> MissionWidgetClass;




	void PopulateGrid();

	void UpdateGridHighlights();

	void HighlightCell(int32 Column, int32 Row, bool bSelected = true);

	void ChangeCurrentCell(int32 ColumnInc, int32 RowInc);

	UBorder* GetCellWidget(int32 Column, int32 Row);


	FIntVector2 CurrentCell = FIntVector2(0);

	TArray<FMissionInfo>	MissionInfos;

	float ExpanderAge = 0.0f;
};
