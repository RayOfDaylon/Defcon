#include "DefconHumansInfoBase.h"
#include "Common/util_color.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/HorizontalBox.h"
#include "UMG/Public/Components/HorizontalBoxSlot.h"


void UDefconHumansInfoBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

#if 0
	// Clear out any existing content and fill with fifteen 10 x 10 images, all but the first with a left padding of 4.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);

	Box->ClearChildren();

	for(int32 Idx = 0; Idx < 15; Idx++)
	{
		auto Image = CreateWidget<UImage>(this);

		auto BoxSlot = Box->AddChildToHorizontalBox(Image);

		BoxSlot->SetPadding(FMargin(Idx == 0 ? 0 : 4, 0, 0, 0));
	}
#endif
}



void UDefconHumansInfoBase::UpdateReadout(const TArray<bool>& AbductionStates)
{
	// Called whenever a human's abduction status has changed.
	// The length of the array indicates the number of surviving humans.

	const int32 Count = AbductionStates.Num();

	// Our root child is a horizontal box, and in that, a set of fifteen small images.

	auto Box = Cast<UHorizontalBox>(GetRootWidget());

	check(Box != nullptr);

	Box->ClearChildren();

	const FLinearColor Colors[2] = { C_GREEN, C_RED };

	for(int32 Idx = 0; Idx < Count; Idx++)
	{
		auto Image = WidgetTree->ConstructWidget<UImage>();

		auto BoxSlot = Box->AddChildToHorizontalBox(Image);
		Image->SetDesiredSizeOverride(FVector2D(10));
		Image->SetBrushTintColor(FSlateColor(Colors[(int32)AbductionStates[Idx]]));

		BoxSlot->SetPadding(FMargin(Idx == 0 ? 0 : 4, 0, 0, 0));
	}
}
