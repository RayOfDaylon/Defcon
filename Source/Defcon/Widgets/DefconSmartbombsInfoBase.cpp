// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconSmartbombsInfoBase.h"
#include "Common/util_color.h"
#include "Globals/MessageMediator.h"
#include "Globals/prefs.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "UMG/Public/Components/HorizontalBoxSlot.h"


void UDefconSmartbombsInfoBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Clear out any existing content and fill with fifteen 10 x 10 images, all but the first with a left padding of 4.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);

	Box->ClearChildren();

	for(int32 Idx = 0; Idx < SMARTBOMB_MAXNUM; Idx++)
	{
		auto Image = WidgetTree->ConstructWidget<UImage>();

		auto BoxSlot = Box->AddChildToHorizontalBox(Image);

		// This has no effect here (sigh).
		Image->SetDesiredSizeOverride(FVector2D(10));

		BoxSlot->SetPadding(FMargin(Idx == 0 ? 0 : 4, 0, 0, 0));
	}

	// Subscribe to smartbomb count
	{
		Defcon::FMessageConsumer MessageConsumer(this, Defcon::EMessageEx::SmartbombCountChanged, 
			[This = TWeakObjectPtr<UDefconSmartbombsInfoBase>(this)](void* Payload)
			{
				if(!This.IsValid())
				{
					return;
				}

				check(Payload != nullptr);

				const int32 Amount = *static_cast<int32*>(Payload);

				check(Amount >= 0);

				const FString Str = FString::Printf(TEXT("%d"), Amount);

				This.Get()->UpdateReadout(Amount);//SmartbombReadout->SetText(FText::FromString(Str));
			}
		);
		Defcon::GMessageMediator.RegisterConsumer(MessageConsumer);
	}
}


void UDefconSmartbombsInfoBase::UpdateReadout(int32 BombCount)
{
	// Called whenever a human's abduction status has changed.
	// The length of the array indicates the number of surviving humans.


	// Our root child is a horizontal box, and in that, a set of fifteen small images.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);


	for(int32 Idx = 0; Idx < SMARTBOMB_MAXNUM; Idx++)
	{
		auto Image = Cast<UImage>(Box->GetChildAt(Idx));

		check(Image != nullptr);

		if(Idx >= BombCount)
		{
			Daylon::Hide(Image);
			continue;
		}
		
		Daylon::Show(Image);

		Image->SetDesiredSizeOverride(FVector2D(10));
		Image->SetBrushTintColor(C_LIGHT);
	}
}
