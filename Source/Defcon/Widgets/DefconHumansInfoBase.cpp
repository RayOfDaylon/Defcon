#include "DefconHumansInfoBase.h"
#include "Common/util_color.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "UMG/Public/Components/HorizontalBoxSlot.h"
#include "Globals/prefs.h"


void UDefconHumansInfoBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Clear out any existing content and fill with fifteen 10 x 10 images, all but the first with a left padding of 4.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);

	Box->ClearChildren();

	for(int32 Idx = 0; Idx < HUMANS_COUNT; Idx++)
	{
		auto Image = WidgetTree->ConstructWidget<UImage>();

		auto BoxSlot = Box->AddChildToHorizontalBox(Image);

		// This has no effect here (sigh).
		//Image->SetDesiredSizeOverride(FVector2D(10));

		BoxSlot->SetPadding(FMargin(Idx == 0 ? 0 : 4, 0, 0, 0));
	}
}


void UDefconHumansInfoBase::UpdateReadout(const TArray<bool>& AbductionStates)
{
	// Called whenever a human's abduction status has changed.
	// The length of the array indicates the number of surviving humans.

	const int32 CurrentHumansCount = AbductionStates.Num();

	// Our root child is a horizontal box, and in that, a set of fifteen small images.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);

	// If we want to use more colors (e.g. yellow to indicate falling)
	// then we need use an enum instead of bool for the abduction states.
	const FLinearColor Colors[2] = { C_GREEN, C_RED };

	for(int32 Idx = 0; Idx < HUMANS_COUNT; Idx++)
	{
		auto Image = Cast<UImage>(Box->GetChildAt(Idx));

		check(Image != nullptr);

		if(Idx >= CurrentHumansCount)
		{
			Daylon::Hide(Image);
			continue;
		}
		
		Daylon::Show(Image);

		Image->SetDesiredSizeOverride(FVector2D(10));
		Image->SetBrushTintColor(FSlateColor(Colors[(int32)AbductionStates[Idx]]));
	}
}
