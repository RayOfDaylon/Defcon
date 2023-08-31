// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"

#include "SDaylonSprite.h"
#include "UDaylonSpriteWidget.generated.h"

// Warning: do NOT use this widget at design time.

UCLASS(BluePrintType)
class DAYLONGRAPHICSLIBRARY_API UDaylonSpriteWidgetAtlas : public UDataAsset
{
	GENERATED_BODY()

	public:

		// The texture holding the sprite cels.
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FDaylonSpriteAtlas Atlas;
};


UCLASS(meta=(DisplayName="Sprite Widget"))
class DAYLONGRAPHICSLIBRARY_API UDaylonSpriteWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

	public:  

		UPROPERTY(EditAnywhere, BlueprintReadonly)
		UDaylonSpriteWidgetAtlas* TextureAtlas;

		FVector2D   Size = FVector2D(16);

		void Tick    (float DeltaTime);
		void Reset   ();

		virtual void SynchronizeProperties () override;
		virtual void ReleaseSlateResources (bool bReleaseChildren) override;

#if WITH_EDITOR
		const FText GetPaletteCategory() override;
#endif

	protected:

		virtual TSharedRef<SWidget> RebuildWidget() override;

		TSharedPtr<SDaylonSprite> MySprite;
};
