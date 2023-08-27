// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
//#include "Runtime/UMG/Public/Components/Border.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "DefconUserWidgetBase.generated.h"


// Base class of all UUserWidgets used as child widgets in a view
UCLASS()
class DEFCON_API UDefconUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

	protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvas;
};


UCLASS()
class DEFCON_API UDefconCellExpanderBase : public UDefconUserWidgetBase
{
	GENERATED_BODY()

	protected:

	//UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	//TObjectPtr<UBorder> Border;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor BorderColor = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BorderWidth = 2.0f;



	virtual int32 NativePaint(
		const FPaintArgs& Args, 
		const FGeometry& AllottedGeometry, 
		const FSlateRect& MyCullingRect, 
		FSlateWindowElementList& OutDrawElements, 
		int32 LayerId, 
		const FWidgetStyle& InWidgetStyle, 
		bool bParentEnabled ) const override;


	//FSlateColorBrush Brush = FSlateColorBrush(BorderColor);


	public:

	//void UpdateBorderWidth();
};


