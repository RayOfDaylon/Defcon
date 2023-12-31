// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Arenas/DefconArena.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "DefconLogging.h"
#include "DefconViewBase.generated.h"





class UDefconGameInstance;


UCLASS()
class DEFCON_API UDefconViewBase : public UUserWidget
{
	GENERATED_BODY()

	public:

	const UDefconGameInstance* GetConstDefconGameInstance() const;

	virtual bool IsOkayToFinishActivating   () const { return true; }
	virtual void OnActivate                 ();
	virtual void OnFinishActivating         ();
	virtual void OnDeactivate               ();
	virtual void OnShiftPressed             (bool IsDown) { ShiftKeyDown = IsDown; }
	virtual void OnEscPressed               () {}
	virtual void OnEnterPressed             () {}
	virtual void OnSkipPressed              () {}
	virtual void OnPausePressed             () {}
	virtual void OnBulletTimePressed        () {}
	virtual void OnNavEvent                 (ENavigationKey Key) {}
	virtual void OnPawnNavEvent             (EDefconPawnNavigationEvent Event, bool Active) {}
	virtual void OnPawnWeaponEvent          (EDefconPawnWeaponEvent Event, bool Active) {}
	virtual void OnToggleDebugStats         () {}
	virtual void OnToggleShowBoundingBoxes  () {}
	virtual void OnToggleShowOrigin         () {}
	virtual bool RequiresGameMatch          () const { return false; }

	protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;


	public:

	UFUNCTION(BlueprintCallable, Category=Defcon)
	UDefconGameInstance* GetDefconGameInstance();


	protected:

	UFUNCTION(BlueprintCallable, Category=Defcon)
	void TransitionToArena(EDefconArena Arena);
	
	// todo: move these audio properties into the audio manager and add their enums, and call GMessageMediator.PlaySound to play them.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> FocusChangedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> InvalidSelectionSound;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* RootCanvas;

	
	float        Age             = 0.0f;
	mutable bool bDoneActivating = false;
	bool         ShiftKeyDown    = false;
};
