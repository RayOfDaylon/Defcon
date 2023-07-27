// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "SDaylonSprite.h"
#include "DaylonUtils.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




void FDaylonSpriteAtlas::InitCache()
{
	UVSize       = FVector2D(1.0 / CelsAcross, 1.0 / CelsDown);
	CelPixelSize = AtlasBrush.GetImageSize() * UVSize;

	LogToPhysCelIndices.Empty(); // to be safe.

	int32 Index = 0;

	for(; Index < NumCels; Index++)
	{
		LogToPhysCelIndices.Add(Index);
	}

	if(bReversible)
	{
		// Add additional indices to support reversible looping.
		// If the number of cels is less than three, no extra indices will be added.

		for(; Index < NumCels + NumCels - 2; Index++)
		{
			LogToPhysCelIndices.Add(Index - (Index - (NumCels - 1)) * 2);
		}
	}
}


bool FDaylonSpriteAtlas::IsValidCelIndex(int32 Index) const
{
	return (Index >= 0 && Index < NumCels);
}


int32 FDaylonSpriteAtlas::CalcCelIndex(int32 CelX, int32 CelY) const
{
	return (CelY * CelsAcross + CelX);
}


FVector2D FDaylonSpriteAtlas::GetCelPixelSize() const
{
	return CelPixelSize;
}


FVector2D FDaylonSpriteAtlas::GetUVSize() const
{
	return UVSize;
}


FBox2d FDaylonSpriteAtlas::GetUVsForCel(int32 Index) const
{
	if(!IsValidCelIndex(Index))
	{
		return FBox2d(FVector2D(0), FVector2D(0));
	}

	return GetUVsForCel(Index % CelsAcross, Index / CelsAcross);
}


FBox2d FDaylonSpriteAtlas::GetUVsForCel(int32 CelX, int32 CelY) const
{
	const FVector2D UV(CelX * UVSize.X, CelY * UVSize.Y);

	return FBox2D(UV, UV + UVSize);
}


// --------------------------------------------------------------------------------------


void SDaylonSprite::Construct(const FArguments& InArgs)
{
	Size = InArgs._Size.Get();
}


FVector2D SDaylonSprite::ComputeDesiredSize(float) const 
{
	return Size; 
}


void SDaylonSprite::SetSize(const FVector2D& InSize)
{
	Size = InSize; 
}


void SDaylonSprite::SetAtlas(const FDaylonSpriteAtlas& InAtlas) 
{
	Atlas = InAtlas;

	Atlas.InitCache();

	Reset();
}


void SDaylonSprite::Reset()
{
	SetCurrentCel(0);

	CurrentAge = 0.0f;
}


void SDaylonSprite::SetCurrentCel(int32 Index)
{
	if(!Atlas.IsValidCelIndex(Index))
	{
		return;
	}

	CurrentCelIndex = Index;
}


void SDaylonSprite::SetCurrentCel(int32 CelX, int32 CelY)
{
	return SetCurrentCel(Atlas.CalcCelIndex(CelX, CelY));
}


void SDaylonSprite::Update(float DeltaTime)
{
	if(IsStatic)
	{
		return;
	}

	// See if we need to change the current cel.

	const int32 NumLogicalCels  = Atlas.LogToPhysCelIndices.Num();
	const float SecondsPerFrame = (1.0f / Atlas.FrameRate); 
	
	// Determine the next absolute cel index as if we were always going forwards.
	const int32 NextLogicalCelIndex = (FMath::RoundToInt(CurrentAge / SecondsPerFrame)) % NumLogicalCels;
	SetCurrentCel(Atlas.LogToPhysCelIndices[NextLogicalCelIndex]);

	CurrentAge += DeltaTime;

	// Wrap the current age so it's always within the animation duration.

	const auto AnimDuration = NumLogicalCels * SecondsPerFrame;

	while(CurrentAge > AnimDuration)
	{
		CurrentAge -= NumLogicalCels / Atlas.FrameRate;
	}

	//UE_LOG(LogSlate, Log, TEXT("sprite widget::update: currentage = %.3f, currentcelindex = %d"), CurrentAge, CurrentCelIndex);
}


int32 SDaylonSprite::OnPaint
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
	if(!IsValid(Atlas.AtlasBrush.GetResourceObject()))
	{
		return LayerId;
	}

	FBox2D uvRegion = Atlas.GetUVsForCel(CurrentCelIndex);

	if(FlipHorizontal)
	{
		Swap(uvRegion.Min.X, uvRegion.Max.X);
	}
	if(FlipVertical)
	{
		Swap(uvRegion.Min.Y, uvRegion.Max.Y);
	}

	Atlas.AtlasBrush.SetUVRegion(uvRegion);

	if(AllottedGeometry.HasRenderTransform())
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			&Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			Atlas.AtlasBrush.TintColor.GetSpecifiedColor() * RenderOpacity * InWidgetStyle.GetColorAndOpacityTint().A);
	}
	else
	{
		const auto GeomSize = AllottedGeometry.GetAbsoluteSize();
		const FPaintGeometry PaintGeometry(AllottedGeometry.GetAbsolutePosition(), GeomSize, 1.0f);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			PaintGeometry,
			&Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			Atlas.AtlasBrush.TintColor.GetSpecifiedColor() * RenderOpacity * InWidgetStyle.GetColorAndOpacityTint().A);
	}

#if 0
	{
		// Draw where P is.
		FLinearColor Red(1.0f, 0.0f, 0.0f, 1.0f);
		const FPaintGeometry PaintGeometry2(AllottedGeometry.GetAbsolutePosition(), FVector2D(4), 1.0f);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			PaintGeometry2,
			&Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			Red * RenderOpacity * InWidgetStyle.GetColorAndOpacityTint().A);
	}
#endif


	return LayerId;
}


// ------------------------------------------------------------------------------------------------------------------------

void SDaylonPolyShield::Construct(const FArguments& InArgs)
{
	Size      = InArgs._Size.Get();
	SpinSpeed = InArgs._SpinSpeed.Get();
	Thickness = InArgs._Thickness.Get();
	NumSides  = InArgs._NumSides.Get();

	SegmentHealth.Reserve(NumSides);

	Reset();
}


FVector2D SDaylonPolyShield::ComputeDesiredSize(float) const 
{
	return Size; 
}


void SDaylonPolyShield::SetSize(const FVector2D& InSize)
{
	Size = InSize; 
}


void SDaylonPolyShield::Reset()
{
	CurrentAge = 0.0f;

	SegmentHealth.SetNum(NumSides);

	int32 N = 0;

	for(auto& Health : SegmentHealth)
	{
		Health = 1.0f;
		//Health = (1.0f / NumSides) * N++;
	}
}


void SDaylonPolyShield::Update(float DeltaTime)
{
	CurrentAge += DeltaTime;

	if(CurrentAge > 1000.f)
	{
		CurrentAge -= 1000.f;
	}
}


int32 SDaylonPolyShield::GetHitSegment(const FVector2D& P1, const FVector2D& P2) const
{
	// P1, P2 are in widget space.

	const auto ShieldAngle = CurrentAge * SpinSpeed;

	if(!Daylon::DoesLineSegmentIntersectCircle(P1, P2, FVector2D(0), Size.X / 2))
	{
		return INDEX_NONE;
	}

	const auto HitVector = (P1 + P2) / 2; // todo: take the middle of the line for now


	//UE_LOG(LogDaylon, Log, TEXT("HitAngle = %.2f"), HitAngle);

	const auto HitAngle = FMath::Wrap(Daylon::Vector2DToAngle(HitVector) - ShieldAngle, 0.0f, 360.0f);

	//UE_LOG(LogDaylon, Log, TEXT("Wrapped HitAngle = %.2f"), HitAngle);

	const int32 Segment = (int32)FMath::Floor(HitAngle / (360.0f / NumSides));

	// No collision if hit segment has no health left.
	check(SegmentHealth.IsValidIndex(Segment));

	return (SegmentHealth[Segment] > 0.0f ? Segment : INDEX_NONE);

	//UE_LOG(LogDaylon, Log, TEXT("HitSegment = %d"), HitSegment);
}


void  SDaylonPolyShield::GetSegmentGeometry(int32 SegmentIndex, FVector2D& P1, FVector2D& P2) const
{
	auto Angle = CurrentAge * SpinSpeed;
	const auto AngleDelta = 360.0f / NumSides;
	const auto Radius = Size * 0.5f;

	Angle += AngleDelta * SegmentIndex;

	P1 = Daylon::AngleToVector2D(Angle) * Radius;
	P2 = Daylon::AngleToVector2D(Angle + AngleDelta) * Radius;
}


float SDaylonPolyShield::GetSegmentHealth(int32 Index) const
{
	if(!SegmentHealth.IsValidIndex(Index))
	{
		return 0.0f;
	}

	return SegmentHealth[Index];
}


void SDaylonPolyShield::SetSegmentHealth(int32 Index, float Health)
{
	UE_LOG(LogDaylon, Log, TEXT("SetSegmentHealth: Index = %d"), Index);

	if(!SegmentHealth.IsValidIndex(Index))
	{
		return;
	}

	SegmentHealth[Index] = Health;
}



int32 SDaylonPolyShield::OnPaint
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
	float Angle = CurrentAge * SpinSpeed;
	const float AngleDelta = 360.0f / NumSides;
	const auto Radius = Size * 0.5f;

	const auto AllottedGeometryLocalSizeHalf = AllottedGeometry.GetLocalSize() / 2;

	for(int32 SegmentIndex = 0; SegmentIndex < NumSides; SegmentIndex++, Angle += AngleDelta)
	{
		if(SegmentHealth[SegmentIndex] <= 0.0f)
		{
			continue;
		}

		FLinearColor Color(1.0f, 1.0f, 1.0f, SegmentHealth[SegmentIndex]);

		TArray<FVector2f> Points;

		Points.Add(UE::Slate::CastToVector2f(AllottedGeometryLocalSizeHalf + Daylon::AngleToVector2D(Angle) * Radius));
		Points.Add(UE::Slate::CastToVector2f(AllottedGeometryLocalSizeHalf + Daylon::AngleToVector2D(Angle + AngleDelta) * Radius));

		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, Color, true, Thickness);
	}

	return LayerId;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
