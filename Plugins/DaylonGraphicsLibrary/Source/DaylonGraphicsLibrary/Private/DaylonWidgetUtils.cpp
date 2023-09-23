// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#include "DaylonWidgetUtils.h"
#include "DaylonGeometry.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Blueprint/SlateBlueprintLibrary.h"


UWidgetTree*  Daylon::WidgetTree = nullptr;
UCanvasPanel* Daylon::RootCanvas = nullptr;


void          Daylon::SetWidgetTree   (UWidgetTree* InWidgetTree) { WidgetTree = InWidgetTree; }
UWidgetTree*  Daylon::GetWidgetTree   () { return WidgetTree; }
void          Daylon::SetRootCanvas   (UCanvasPanel* InCanvas) { RootCanvas = InCanvas; }
UCanvasPanel* Daylon::GetRootCanvas   () { return RootCanvas; }



FVector2D Daylon::GetWidgetDirectionVector(const UWidget* Widget)
{
	if (Widget == nullptr)
	{
		return FVector2D(0.0f);
	}

	return Daylon::AngleToVector2D(Widget->GetRenderTransformAngle());
}


void Daylon::Show(UWidget* Widget, bool Visible)
{
	if(Widget == nullptr)
	{
		return;
	}

	Widget->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}


void Daylon::Hide(UWidget* Widget) 
{
	Show(Widget, false); 
}


void Daylon::Show(SWidget* Widget, bool Visible)
{
	if(Widget == nullptr)
	{
		return;
	}

	Widget->SetVisibility(Visible ? EVisibility::HitTestInvisible : EVisibility::Collapsed);
}


void Daylon::Hide(SWidget* Widget) 
{
	Show(Widget, false); 
}


void Daylon::UpdateRoundedReadout(UTextBlock* Readout, float Value, int32& OldValue)
{
	check(Readout);

	const int32 N = FMath::RoundToInt(Value);

	if(N != OldValue)
	{
		const auto Str = FString::Printf(TEXT("%d"), N);
		Readout->SetText(FText::FromString(Str));
		OldValue = N;
	}
}


FVector2D Daylon::GetWidgetPosition(const UWidget* Widget)
{
	// Return a UWidget's position in layout (not screen) space. E.g. an HD canvas on a 4K screen will return HD units
#if 1
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(const_cast<UWidget*>(Widget), Widget->GetCachedGeometry(), FVector2D(0), PixelPosition, ViewportPosition);
	return ViewportPosition;
#else
	const auto& Geometry = Widget->GetPaintSpaceGeometry();

	return Geometry.GetAbsolutePosition() / Geometry.Scale;
#endif
}


FVector2D Daylon::GetWidgetSize(const UWidget* Widget)
{
	// Return a UWidget's size in layout (not screen) space. E.g. an HD canvas on a 4K screen will return HD units

	const auto& Geometry = Widget->GetPaintSpaceGeometry();

	return Geometry.GetAbsoluteSize() / Geometry.Scale;
}
