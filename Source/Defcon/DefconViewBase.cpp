// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconViewBase.h"
#include "DefconUtils.h"
#include "DefconLogging.h"


const UDefconGameInstance* UDefconViewBase::GetConstDefconGameInstance() const
{
	return gDefconGameInstance;
}


UDefconGameInstance* UDefconViewBase::GetDefconGameInstance()
{
	return gDefconGameInstance;
}


void UDefconViewBase::TransitionToArena(EDefconArena Arena)
{
	gDefconGameInstance->TransitionToArena(Arena);
}


int32 UDefconViewBase::NativePaint
(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	LayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);

	if(!bDoneActivating && gDefconGameInstance != nullptr && gDefconGameInstance->IsLive())
	{
		const_cast<UDefconViewBase*>(this)->OnFinishActivating();

		bDoneActivating = true;
	}

	return LayerId;
}
