// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconViewBase.h"
#include "DefconUtils.h"
#include "DefconLogging.h"


const UDefconGameInstance* UDefconViewBase::GetConstDefconGameInstance() const
{
	return GDefconGameInstance;
}


UDefconGameInstance* UDefconViewBase::GetDefconGameInstance()
{
	return GDefconGameInstance;
}


void UDefconViewBase::TransitionToArena(EDefconArena Arena)
{
	GDefconGameInstance->TransitionToArena(Arena);
}


#if 0
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

	// Tracking non-design-time paint events indicates that view widget children have finished setting up and have sizes,
	// and therefore we can safely finish activating from NativeTick.


	if(!bPaintingOccurring)
	{
#if WITH_EDITOR
		const bool InDesignTime = IsDesignTime();
#else
		const bool InDesignTime = false;
#endif
		if(!InDesignTime)
		{
			bPaintingOccurring = true;
		}
	}

	return LayerId;
}
#endif


void UDefconViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Age += DeltaTime;

	if(!bDoneActivating && IsOkayToFinishActivating())
	{
		// This test is probably not needed, but just to be safe.
		if(GDefconGameInstance != nullptr && GDefconGameInstance->IsLive() && GDefconGameInstance->GetCurrentView() == this)
		{
			OnFinishActivating();

			bDoneActivating = true;
		}
	}
}


void UDefconViewBase::OnActivate()
{
	check(!bDoneActivating);

	Age = 0.0f;

	ShiftKeyDown = false;
}


void UDefconViewBase::OnFinishActivating()
{
}


void UDefconViewBase::OnDeactivate()
{
	// todo: this check can fail sometimes if the view didn't get to tick.
	// We need to revisit our strategy again for view activation/deactivation, maybe forcing 
	// Tick to not happen until BeginPlay would help.
	//check(bDoneActivating);

	bDoneActivating    = false;
}

