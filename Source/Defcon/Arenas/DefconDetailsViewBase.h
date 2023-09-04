// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "GameObjects/obj_types.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/GridPanel.h"
#include "Widgets/UDaylonSpilltext.h"
#include "DaylonUtils.h"
#include "DefconDetailsViewBase.generated.h"



struct FDetailsInfo
{
	FText            Text;
	Defcon::EObjType ObjType;
};


UCLASS()
class DEFCON_API UDefconDetailsViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

#if 0
	virtual void NativeOnInitialized() override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;


	virtual void OnActivate   () override;
#endif

	virtual void OnFinishActivating  () override;
	virtual void OnDeactivate        () override;

	virtual void OnEscPressed        () override;
	virtual void OnSkipPressed       () override;
	virtual void OnEnterPressed      () override;

	void ShowPage        (int32 Idx);
	void ShowNextPage    ();
	void UninstallSprite ();


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDaylonSpilltext* TextReadout;

	TArray<FDetailsInfo> Pages;
	int32                CurrentPageIdx = -1;

	TSharedPtr<Daylon::SpritePlayObject2D> Sprite;

	bool SafeToTick = false;
};
