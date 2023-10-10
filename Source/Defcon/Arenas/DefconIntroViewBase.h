// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/SLerpedLines.h"
#include "UMG/Public/Components/TextBlock.h"
#include "DefconIntroViewBase.generated.h"


UCLASS()
class DEFCON_API UDefconIntroViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* CompanyName;

	virtual void NativeOnInitialized  () override;
	virtual void NativeTick           (const FGeometry& MyGeometry, float DeltaTime) override;

	virtual void OnEnterPressed () override;
	virtual void OnEscPressed   () override;


	TSharedPtr<SDaylonLerpedLines> TitleWidget;

	TArray<FDaylonLerpedLine> TitleLines;

	bool bFirstTime                = true;
};
