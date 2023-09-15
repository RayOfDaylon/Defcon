// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "SlateCore/Public/Widgets/SLeafWidget.h"
#include "SDaylonSprite.generated.h"



// SDaylonSpriteWidget - a drawing widget with a custom Paint event.
// If your sprite is animated, call Update().
// If you just want to switch amongst cels, call SetCurrentCel().


USTRUCT(BlueprintType)
struct DAYLONGRAPHICSLIBRARY_API FDaylonSpriteAtlas
{
	GENERATED_USTRUCT_BODY()

	FDaylonSpriteAtlas() {}

	// Texture containing a flipbook of one or more cels.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush AtlasBrush;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	int32 CelsAcross = 1;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	int32 CelsDown = 1;

	// Total number of cels to use (starting at cel 0,0)
	UPROPERTY(EditAnywhere, BlueprintReadonly)
	int32 NumCels = 1;

	// If using the atlas to animate a sprite, set a frame rate.
	UPROPERTY(EditAnywhere, BlueprintReadonly)
	float FrameRate = 30.0f;

	// If checked, plays the animation forwards, then backwards, then forwards, etc.
	UPROPERTY(EditAnywhere, BlueprintReadonly)
	bool bReversible = false;

	FVector2D      CelPixelSize;
	FVector2D      UVSize;

	// This array maps logical cel indices to physical ones.
	// E.g. if you have a reversible four cel atlas, the array
	// will hold six indices: [ 0 1 2 3  2 1 ]. During rendering, 
	// a logical cel index for the current frame will be computed, 
	// modulated to the 0-5 range, then lookup into the array
	// to get the physical cel index. This lets the sprite treat 
	// the atlas as if it had the extra cels, simplifying its logic.
	// All other mentions of cel indices refer to physical indices.

	TArray<int32>  LogToPhysCelIndices;


	void       InitCache        ();
			   
	bool       IsValidCelIndex  (int32 Index) const;

	int32      CalcCelIndex     (int32 CelX, int32 CelY) const;
			   
	FVector2D  GetCelPixelSize  () const;
	FVector2D  GetUVSize        () const;
			   
	FBox2d     GetUVsForCel     (int32 Index) const;
	FBox2d     GetUVsForCel     (int32 CelX, int32 CelY) const;
};


class DAYLONGRAPHICSLIBRARY_API SDaylonSprite : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDaylonSprite)
			: 
			  _Size                (FVector2D(16))
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D, Size)

			SLATE_END_ARGS()

			SDaylonSprite() {}

			~SDaylonSprite() {}

			void Construct(const FArguments& InArgs);


			const FVector2D&  GetSize       () const { return Size; }
			void              SetSize       (const FVector2D& InSize);
			void              SetTint       (const FLinearColor& Color) { Tint = Color; }
			void              SetAtlas      (const FDaylonSpriteAtlas& InAtlas);

			void              SetCurrentCel (int32 Index);                      // Useful only in static mode
			void              SetCurrentCel (int32 CelX, int32 CelY);
			void              SetCurrentAge (float Age) { CurrentAge = Age; }   // Useful in dynamic mode
			void              Update        (float DeltaTime);
			void              Reset         ();


			virtual int32 OnPaint
			(
				const FPaintArgs&          Args,
				const FGeometry&           AllottedGeometry,
				const FSlateRect&          MyCullingRect,
				FSlateWindowElementList&   OutDrawElements,
				int32                      LayerId,
				const FWidgetStyle&        InWidgetStyle,
				bool                       bParentEnabled
			) const override;

			virtual FVector2D ComputeDesiredSize(float) const override;

			bool      IsStatic       = false; // Set to true to use current cel regardless of updating
			bool      FlipHorizontal = false;
			bool      FlipVertical   = false;


		protected:

			FLinearColor                 Tint = FLinearColor::White;
			FVector2D                    Size;
			mutable FDaylonSpriteAtlas   Atlas;

			float                        CurrentAge      = 0.0f;
			int32                        CurrentCelIndex = 0;

			virtual bool ComputeVolatility() const override { return true; }

};



class DAYLONGRAPHICSLIBRARY_API SDaylonPolyShield : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDaylonPolyShield)
			: 
			  _Size                (FVector2D(16)),
			  _SpinSpeed           (10.0f),
			  _Thickness           (2.0f),
			  _NumSides            (9)
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D, Size)
			SLATE_ATTRIBUTE(float,     SpinSpeed)
			SLATE_ATTRIBUTE(float,     Thickness)
			SLATE_ATTRIBUTE(int32,     NumSides)

			SLATE_END_ARGS()

			SDaylonPolyShield() {}

			~SDaylonPolyShield() {}

			void Construct(const FArguments& InArgs);


			void              SetSize      (const FVector2D& InSize);
			FVector2D         GetSize      () const { return Size; }
			int32             GetNumSides  () const { return NumSides; }
			float             GetThickness () const { return Thickness; }

			void Update        (float DeltaTime);
			void Reset         ();

			// Given P1 and P2 in local coordinates, return which segment got hit (INDEX_NONE if no hit).
			int32 GetHitSegment      (const FVector2D& P1, const FVector2D& P2) const;
			void  SetSegmentHealth   (int32 Index, float Health);
			float GetSegmentHealth   (int32 Index) const;
			void  GetSegmentGeometry (int32 SegmentIndex, FVector2D& P1, FVector2D& P2) const; // Local space



			virtual int32 OnPaint
			(
				const FPaintArgs&          Args,
				const FGeometry&           AllottedGeometry,
				const FSlateRect&          MyCullingRect,
				FSlateWindowElementList&   OutDrawElements,
				int32                      LayerId,
				const FWidgetStyle&        InWidgetStyle,
				bool                       bParentEnabled
			) const override;

			virtual FVector2D ComputeDesiredSize(float) const override;



		protected:

			FVector2D                    Size;
			float                        SpinSpeed;
			float                        Thickness;
			int32                        NumSides;
			TArray<float>                SegmentHealth;

			float                        CurrentAge      = 0.0f;

			virtual bool ComputeVolatility() const override { return true; }

};
