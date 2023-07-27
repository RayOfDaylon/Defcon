// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DaylonUtils.h"
#include "UDaylonSpriteWidget.h"
#include "Common/PaintArguments.h"
#include "GameObjects/gameobj.h"
#include "GameObjects/gameobjlive.h"
#include "DefconLogging.h"
#include "MainViewBase.generated.h"




struct FSprite2D
{
	// A non-widget version of SDaylonSprite we can use when issuing render commands w/o Slate

	virtual ~FSprite2D() {}

	//const FVector2D&  GetSize       () const { return Size; }
	//void              SetSize       (const FVector2D& InSize);
	void                SetAtlas      (const FDaylonSpriteAtlas& InAtlas);
	void                SetCurrentCel (int32 Index);
	void                SetCurrentCel (int32 CelX, int32 CelY);
	void                Tick          (float DeltaTime);
	void                Reset         ();


	virtual int32 Paint(const FPaintArguments& FrameBuffer) const;


	protected:

		mutable FDaylonSpriteAtlas   Atlas;
		float                        CurrentAge      = 0.0f;
		int32                        CurrentCelIndex = 0;
};



/**
 * 
 */
UCLASS()
class DEFCON_API UMainViewBase : public UUserWidget
{
	GENERATED_BODY()

	protected:

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

	// ---------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = SpaceRox)
	void OnFireButtonPressed();

	UFUNCTION(BlueprintCallable, Category = SpaceRox)
	void OnMoveVertically(float Value);

	// ---------------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Objects)
	UDaylonSpriteWidgetAtlas* ExplosionAtlas;


	// ---------------------------------------------------------------------------------
	FSprite2D TestSprite;
};
