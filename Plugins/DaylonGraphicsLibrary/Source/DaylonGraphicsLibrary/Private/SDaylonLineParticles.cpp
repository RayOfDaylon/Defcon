// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "SDaylonLineParticles.h"
#include "DaylonGeometry.h"
#include "DaylonRNG.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


void SDaylonLineParticles::Construct(const FArguments& InArgs)
{
	Size                = InArgs._Size.Get();
	MinParticleVelocity = InArgs._MinParticleVelocity.Get();
	MaxParticleVelocity = InArgs._MaxParticleVelocity.Get();
	MinParticleLifetime = InArgs._MinParticleLifetime.Get();
	MaxParticleLifetime = InArgs._MaxParticleLifetime.Get();
	LineThickness       = InArgs._LineThickness.Get();
	FinalOpacity        = InArgs._FinalOpacity.Get();
}


void SDaylonLineParticles::Set(const FDaylonLineParticlesParams& Params)
{
	MinParticleVelocity = Params.MinParticleVelocity;
	MaxParticleVelocity = Params.MaxParticleVelocity;
	MinParticleLifetime = Params.MinParticleLifetime;
	MaxParticleLifetime = Params.MaxParticleLifetime;
	LineThickness       = Params.LineThickness;
	FinalOpacity        = Params.FinalOpacity;

	SetParticles(Params.Particles);
}


void SDaylonLineParticles::SetParticles(const TArray<FDaylonLineParticle>& InParticles) 
{ 
	Particles = InParticles;

	for(auto& Particle : Particles)
	{
		// Make each line segment fly off along its vector to the center with a little random deviation.
		const auto Angle = Daylon::Vector2DToAngle(Particle.P) + Daylon::FRandRange(-15.0f, 15.0f);

		Particle.Inertia       = /*Daylon::RandVector2D()*/ 
			Daylon::AngleToVector2D(Angle) 
			* Daylon::FRandRange(MinParticleVelocity, MaxParticleVelocity);
			
		Particle.LifeRemaining = Particle.StartingLifeRemaining = Daylon::FRandRange(MinParticleLifetime, MaxParticleLifetime);
	}
}


bool SDaylonLineParticles::Update(float DeltaTime)
{
	bool Alive = false;

	for(auto& Particle : Particles)
	{
		Alive |= Particle.Update(DeltaTime);
	}

	return Alive;
}


int32 SDaylonLineParticles::OnPaint
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
	for(const auto& Particle : Particles)
	{
		if(Particle.LifeRemaining <= 0.0f)
		{
			continue;
		}

		// Draw the line particle.

		// Overdrive starting opacity so that we don't start darkening right away.
		float CurrentOpacity = FMath::Lerp(FinalOpacity, 1.5f, Particle.LifeRemaining / Particle.StartingLifeRemaining);
		CurrentOpacity = FMath::Min(1.0f, CurrentOpacity);

		FLinearColor Color = Particle.Color;
		Color.A = CurrentOpacity;

		TArray<FVector2f> Points;

		const auto AngleVec = Daylon::AngleToVector2D(Particle.Angle) * Particle.Length / 2;

		const auto P1 = Particle.P + AngleVec;
		const auto P2 = Particle.P - AngleVec;

		Points.Add(UE::Slate::CastToVector2f(P1));
		Points.Add(UE::Slate::CastToVector2f(P2));

		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Points, ESlateDrawEffect::None, Color, true, LineThickness);
	}

	return LayerId;
}


FVector2D SDaylonLineParticles::ComputeDesiredSize(float) const 
{
	return Size; 
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
