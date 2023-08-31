// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMG/Public/Components/Widget.h"
#include "SDaylonStarfield.h"
#include "UDaylonStarfield.generated.h"

/* 
	UDaylonStarfield - UMG wrapper around SDaylonStarfield.
*/
UCLASS(meta=(DisplayName="Starfield Widget"))
class DEFCON_API UDaylonStarfield : public UWidget
{
	GENERATED_UCLASS_BODY()

	public:

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		FVector2D Size;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		int32 Count;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float Speed = 1.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float RotationSpeed = 0.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		FSlateBrush  Brush;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float Age = 0.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float FadeStartsAt = 0.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float FadeEndsAt = 30000.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float OsStart = 1.0f;

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		float OsEnd = 0.0f;


		void SetColors(const TArray<FLinearColor>& InColors);

		virtual void SynchronizeProperties () override;
		virtual void ReleaseSlateResources (bool bReleaseChildren) override;

		void Tick(float DeltaTime);

#if WITH_EDITOR
		// UWidget interface
		virtual const FText GetPaletteCategory() override;
#endif


	protected:

		virtual TSharedRef<SWidget> RebuildWidget() override;

		TSharedPtr<SDaylonStarfield> MyStarfield;
};

