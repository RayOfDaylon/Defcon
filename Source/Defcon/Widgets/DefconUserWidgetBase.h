// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
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
