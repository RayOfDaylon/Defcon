// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "UMG/Public/Components/Widget.h"
#include "SDaylonProgressBar.h"
#include "UDaylonProgressBar.generated.h"


UCLASS(meta=(DisplayName="Daylon Progress Bar Widget"))
class DEFCON_API UDaylonProgressBar : public UWidget
{
	GENERATED_UCLASS_BODY()


	protected:

		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "0", UIMax = "1"))
		float Percent;

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FLinearColor FillColorAndOpacity;

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BorderThickness;

	public:

		float GetPercent() const;

		UFUNCTION(BlueprintCallable, Category="Progress")
		void SetPercent(float InPercent);

		FLinearColor GetFillColorAndOpacity() const;

		UFUNCTION(BlueprintCallable, Category="Progress")
		void SetFillColorAndOpacity(FLinearColor InColor);

	
		//~ Begin UWidget Interface
		virtual void SynchronizeProperties() override;
		//~ End UWidget Interface

		//~ Begin UVisual Interface
		virtual void ReleaseSlateResources(bool bReleaseChildren) override;
		//~ End UVisual Interface

#if WITH_EDITOR
		//~ Begin UWidget Interface
		virtual const FText GetPaletteCategory() override;
		virtual void OnCreationFromPalette() override;
		//~ End UWidget Interface
#endif

	protected:
		/** Native Slate Widget */
		TSharedPtr<SDaylonProgressBar> MyProgressBar;

		//~ Begin UWidget Interface
		virtual TSharedRef<SWidget> RebuildWidget() override;
		//~ End UWidget Interface
};

