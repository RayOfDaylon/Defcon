// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Arenas/DefconArena.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "DefconViewBase.generated.h"


class UDefconGameInstance;


UCLASS()
class DEFCON_API UDefconViewBase : public UUserWidget
{
	GENERATED_BODY()

	public:

	const UDefconGameInstance* GetConstDefconGameInstance() const;

	virtual void OnActivate                 () { Age = 0.0f; bDoneActivating = false; bFirstTime = true;  }
	virtual void OnFinishActivating         () {}
	virtual void OnDeactivate               () {}
	virtual void OnEscPressed               () {}
	virtual void OnEnterPressed             () {}
	virtual void OnSkipPressed              () {}
	virtual void OnNavEvent                 (ENavigationKey Key) {}
	virtual void OnPawnNavEvent             (EDefconPawnNavigationEvent Event, bool Active) {}
	virtual void OnPawnWeaponEvent          (EDefconPawnWeaponEvent Event, bool Active) {}
	virtual void OnToggleDebugStats         () {}
	virtual void OnToggleShowBoundingBoxes  () {}
	virtual void OnToggleShowOrigin         () {}

	protected:

	//virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	public:
	UFUNCTION(BlueprintCallable, Category=Defcon)
	UDefconGameInstance* GetDefconGameInstance();

	protected:
	UFUNCTION(BlueprintCallable, Category=Defcon)
	void TransitionToArena(EDefconArena Arena);
	// todo: move these audio properties into the audio manager and add their enums, and call gpAudio->OutputSound to play them.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> FocusChangedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> InvalidSelectionSound;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* RootCanvas;


	bool bFirstTime = true;
	
	float Age = 0.0f;
	mutable bool bDoneActivating = false;
};
