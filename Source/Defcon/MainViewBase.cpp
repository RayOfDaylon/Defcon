// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "MainViewBase.h"

#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"
#include "SlateCore/Public/Widgets/SLeafWidget.h"





void FSprite2D::SetAtlas(const FDaylonSpriteAtlas& InAtlas) 
{
	Atlas = InAtlas; 
	Atlas.InitCache(); 
	Reset(); 
}


void FSprite2D::Reset() 
{
	SetCurrentCel(0); 
	CurrentAge = 0.0f; 
}


void FSprite2D::SetCurrentCel(int32 Index)
{
	if(!Atlas.IsValidCelIndex(Index))
	{
		return;
	}

	CurrentCelIndex = Index;
}


void FSprite2D::SetCurrentCel(int32 CelX, int32 CelY)
{
	return SetCurrentCel(Atlas.CalcCelIndex(CelX, CelY));
}


void FSprite2D::Tick(float DeltaTime)
{
	const auto SecondsPerFrame = 1.0f / Atlas.FrameRate;

	SetCurrentCel((FMath::RoundToInt(CurrentAge / SecondsPerFrame)) % Atlas.NumCels);

	CurrentAge += DeltaTime;

	const auto AnimDuration = Atlas.NumCels * SecondsPerFrame;

	while(CurrentAge > AnimDuration)
	{
		CurrentAge -= Atlas.NumCels / Atlas.FrameRate;
	}

	//UE_LOG(LogSlate, Log, TEXT("sprite widget::update: currentage = %.3f, currentcelindex = %d"), CurrentAge, CurrentCelIndex);
}


int32 FSprite2D::Paint(const FPaintArguments& Args) const
{
	const FBox2D uvRegion = Atlas.GetUVsForCel(CurrentCelIndex);

	Atlas.AtlasBrush.SetUVRegion(uvRegion);

	const auto GeomSize = Args.AllottedGeometry->GetAbsoluteSize();
	const FPaintGeometry PaintGeometry(Args.AllottedGeometry->GetAbsolutePosition(), GeomSize, 1.0f);

	FSlateDrawElement::MakeBox(
		*Args.OutDrawElements,
		Args.LayerId,
		PaintGeometry,
		&Atlas.AtlasBrush,
		ESlateDrawEffect::None,
		Atlas.AtlasBrush.TintColor.GetSpecifiedColor() * Args.RenderOpacity);

	return Args.LayerId;
}


// --------------------------------------------------------------------------------------------------------------------------------------------

void UMainViewBase::NativeOnInitialized()
{
	TestSprite.SetAtlas(ExplosionAtlas->Atlas);
	//ExplosionAtlas->Atlas.InitCache();
}


void UMainViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	//UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);	

	TestSprite.Tick(DeltaTime);
}


int32 UMainViewBase::NativePaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
#if 0
	// Test: issue render calls directly.

	const FSlateLayoutTransform Translation(FVector2D(200, 100));
	auto Geometry = AllottedGeometry.MakeChild(FVector2D(64), Translation);

	const FPaintArguments PaintArguments = 
	{
		&Args,
		&Geometry,
		nullptr,
		&MyCullingRect,
		&OutDrawElements,
		LayerId,
		bParentEnabled,
		InWidgetStyle.GetColorAndOpacityTint().A
	};

	return TestSprite.Paint(PaintArguments);
#endif
}




void UMainViewBase::OnFireButtonPressed()
{
}


void UMainViewBase::OnMoveVertically(float Value)
{
}
