// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "UDaylonProgressBar.h"
//#include "Slate/SlateBrushAsset.h"
//#include "Styling/UMGCoreStyle.h"

//#include UE_INLINE_GENERATED_CPP_BY_NAME(ProgressBar)

//#define LOCTEXT_NAMESPACE "UMG"



UDaylonProgressBar::UDaylonProgressBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR 
	if (IsEditorWidget())
	{
		// The CDO isn't an editor widget and thus won't use the editor style, call post edit change to mark difference from CDO
		PostEditChange();
	}
#endif // WITH_EDITOR

	Percent             = 0;
	FillColorAndOpacity = FLinearColor::White;
	BorderThickness     = 0;
}


void UDaylonProgressBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyProgressBar.Reset();
}


TSharedRef<SWidget> UDaylonProgressBar::RebuildWidget()
{
	MyProgressBar = SNew(SDaylonProgressBar);

	return MyProgressBar.ToSharedRef();
}


void UDaylonProgressBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	//TAttribute< TOptional<float> > PercentBinding = OPTIONAL_BINDING_CONVERT(float, Percent, TOptional<float>, ConvertFloatToOptionalFloat);
	//TAttribute<FSlateColor> FillColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, FillColorAndOpacity);

	MyProgressBar->SetPercent(Percent);
	MyProgressBar->SetFillColorAndOpacity(FillColorAndOpacity);
	MyProgressBar->SetBorderThickness(BorderThickness);
}


float UDaylonProgressBar::GetPercent() const
{
	return Percent;
}


void UDaylonProgressBar::SetPercent(float InPercent)
{
	Percent = InPercent;

	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetPercent(InPercent);
	}
}


FLinearColor UDaylonProgressBar::GetFillColorAndOpacity() const
{
	return FillColorAndOpacity;
}

void UDaylonProgressBar::SetFillColorAndOpacity(FLinearColor Color)
{
	FillColorAndOpacity = Color;

	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}


#if WITH_EDITOR

const FText UDaylonProgressBar::GetPaletteCategory()
{
	return FText::FromString(TEXT("Daylon"));
}

void UDaylonProgressBar::OnCreationFromPalette()
{
	FillColorAndOpacity = FLinearColor::White;
}

#endif


//#undef LOCTEXT_NAMESPACE

