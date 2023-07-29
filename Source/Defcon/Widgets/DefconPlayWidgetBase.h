// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconUserWidgetBase.h"
#include "UDaylonSpriteWidget.h"
#include "DefconPlayWidgetBase.generated.h"


// Base class of all UUserWidgets used as child widgets in the Play arena e.g. stats/radar/main
UCLASS()
class DEFCON_API UDefconPlayWidgetBase : public UDefconUserWidgetBase
{
	GENERATED_BODY()


	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs&          Args,
		const FGeometry&           AllottedGeometry,
		const FSlateRect&          MyCullingRect,
		FSlateWindowElementList&   OutDrawElements,
		int32                      LayerId,
		const FWidgetStyle&        InWidgetStyle,
		bool                       bParentEnabled) const override;



};


