// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "SlateCore/Public/Widgets/SLeafWidget.h"



// SDaylonLineParticles - a Slate widget with a custom Paint event.
// Used to draw e.g. disentegrating polyshields in Stellar Mayhem.
// After instancing, call Set() with a FDaylonLineParticlesParams object
// which holds all the initial line segment data.


struct DAYLONGRAPHICSLIBRARY_API FDaylonLineParticle
{
	// A single line particle.
	// The widget maintains an array of these.

	FVector2D     P          = FVector2D(0); // Line centerpoint
	FVector2D     Inertia    = FVector2D(0);
	float         Length     = 10.0f;
	float         Angle      = 1.0f;         // Rotation about centerpoint
	float         Spin       = 0.0f;         // Degrees per second
	FLinearColor  Color      = FLinearColor::White;

	float     LifeRemaining = 0.0f;         // Modulates opacity
	float     StartingLifeRemaining;

	bool Update(float DeltaTime)
	{
		Angle += Spin * DeltaTime;
		P += Inertia * DeltaTime;
		LifeRemaining -= DeltaTime;

		return (LifeRemaining > 0.0f);
	}
};


struct DAYLONGRAPHICSLIBRARY_API FDaylonLineParticlesParams
{
	TArray<FDaylonLineParticle> Particles;

	float MinParticleVelocity;
	float MaxParticleVelocity;
	float MinParticleLifetime;
	float MaxParticleLifetime;
	float LineThickness;
	float FinalOpacity;
};


class DAYLONGRAPHICSLIBRARY_API SDaylonLineParticles : public SLeafWidget
{
	public:
		SLATE_BEGIN_ARGS(SDaylonLineParticles)
			: 
			  _Size                (FVector2D(4))
			, _MinParticleVelocity (1.0f)
			, _MaxParticleVelocity (10.0f)
			, _MinParticleLifetime (0.5f)
			, _MaxParticleLifetime (1.5f)
			, _LineThickness       (1.0f)
			, _FinalOpacity        (1.0f)
			{
				_Clipping = EWidgetClipping::OnDemand;
			}

			SLATE_ATTRIBUTE(FVector2D, Size)
			SLATE_ATTRIBUTE(float,     MinParticleVelocity)
			SLATE_ATTRIBUTE(float,     MaxParticleVelocity)
			SLATE_ATTRIBUTE(float,     MinParticleLifetime)
			SLATE_ATTRIBUTE(float,     MaxParticleLifetime)
			SLATE_ATTRIBUTE(float,     LineThickness)
			SLATE_ATTRIBUTE(float,     FinalOpacity)

			SLATE_END_ARGS()

			SDaylonLineParticles() {}

			~SDaylonLineParticles() {}

			void Construct(const FArguments& InArgs);

			void Set                      (const FDaylonLineParticlesParams& Params);
			void SetParticles             (const TArray<FDaylonLineParticle>& InParticles);
			void SetSize                  (const FVector2D& InSize);
			void SetMinParticleVelocity   (float Velocity);
			void SetMaxParticleVelocity   (float Velocity);
			void SetMinParticleLifetime   (float Lifetime);
			void SetMaxParticleLifetime   (float Lifetime);
			void SetFinalOpacity          (float Opacity);
			void SetParticleBrush         (const FSlateBrush& InBrush);

			bool Update                   (float DeltaTime);
			void Reset                    ();


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

			TArray<FDaylonLineParticle> Particles;
			FVector2D                   Size;
			float                       LineThickness;
			float                       MinParticleVelocity;
			float                       MaxParticleVelocity;
			float                       MinParticleLifetime;
			float                       MaxParticleLifetime;
			float                       FinalOpacity;

			virtual bool ComputeVolatility() const override { return true; }

		private:

			float                       StartingLifetime;
};
