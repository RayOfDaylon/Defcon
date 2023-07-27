// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "DaylonUtils.h"
#include "Widgets/UDaylonStarfield.h"
#include "UMG/Public/Components/VerticalBox.h"
#include "UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "DefconMainMenuViewBase.generated.h"



UCLASS()
class DEFCON_API UDefconMainMenuViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	protected:

	// UUserWidget
	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;


	// UDefconViewBase
	virtual void OnActivate     () override;
	virtual void OnEnterPressed () override;
	virtual void OnNavEvent     (ENavigationKey Key) override;


	void NavigateMenu(Daylon::EListNavigationDirection Direction);

	void UpdateMenuReadout();

	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* Menu;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDaylonStarfield* Starfield;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//FSlateBrush StarfieldBrush;


	void OnChooseSelectedItem();

	int32 CurrentItem = 0;

	//TSharedPtr<SDaylonStarfield> Starfield;
};
