// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconMainMenuViewBase.h"
#include "DefconGameInstance.h"
#include "DefconLogging.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"

// -----------------------------------------------------------------------------------------


void UDefconMainMenuViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();

	CurrentItem = 0;
	UpdateMenuReadout();
}


#if 0
void UDefconMainMenuViewBase::NativeOnInitialized()
{
	LOG_UWIDGET_FUNCTION
	// Read the title vector file and set up the lerped lines widget.

	Super::NativeOnInitialized();
}
#endif


void UDefconMainMenuViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);

	Starfield->Tick(DeltaTime);
}


#if 0
int32 UDefconMainMenuViewBase::NativePaint
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
	LOG_UWIDGET_FUNCTION
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}
#endif


void UDefconMainMenuViewBase::UpdateMenuReadout()
{
	auto Children = Menu->GetAllChildren();

	int32 Index = 0;

	for(auto Item : Children)
	{
		auto TextBlock = Cast<UTextBlock>(Item);
		if(TextBlock == nullptr)
		{
			continue;
		}

		TextBlock->SetColorAndOpacity(Index == CurrentItem ? C_WHITE : C_DARKER);
		Index++;
	}
}


void UDefconMainMenuViewBase::NavigateMenu(Daylon::EListNavigationDirection Direction)
{
	GAudio->OutputSound(Defcon::EAudioTrack::Focus_changed);

	CurrentItem += (int32)Direction;
	
	CurrentItem = (CurrentItem + 1000) % Menu->GetChildrenCount();

	UpdateMenuReadout();
}


void UDefconMainMenuViewBase::OnNavEvent(ENavigationKey Key)
{
	switch(Key)
	{
		case ENavigationKey::Up:   NavigateMenu(Daylon::EListNavigationDirection::Backwards); break;
		case ENavigationKey::Down: NavigateMenu(Daylon::EListNavigationDirection::Forwards); break;
	}
}


void UDefconMainMenuViewBase::OnChooseSelectedItem()
{
	switch(CurrentItem)
	{
		case 0: TransitionToArena(EDefconArena::MissionPicker);	break;
		case 1: TransitionToArena(EDefconArena::Help);	        break;
		case 2: TransitionToArena(EDefconArena::Credits);       break;

		case 3: // exit
			UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
			break;
	}
}


void UDefconMainMenuViewBase::OnEnterPressed()
{
	OnChooseSelectedItem();
}
