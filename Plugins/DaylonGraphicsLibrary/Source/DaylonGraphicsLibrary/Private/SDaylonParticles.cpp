// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "SDaylonParticles.h"
#include "DaylonUtils.h"

#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


void SDaylonParticles::Construct(const FArguments& InArgs)
{
	Size = InArgs._Size.Get();
	MinParticleSize     = InArgs._MinParticleSize.Get();
	MaxParticleSize     = InArgs._MaxParticleSize.Get();
	MinParticleVelocity = InArgs._MinParticleVelocity.Get();
	MaxParticleVelocity = InArgs._MaxParticleVelocity.Get();
	MinParticleLifetime = InArgs._MinParticleLifetime.Get();
	MaxParticleLifetime = InArgs._MaxParticleLifetime.Get();
	FinalOpacity        = InArgs._FinalOpacity.Get();

	SetNumParticles(InArgs._NumParticles.Get());
}


FVector2D SDaylonParticles::ComputeDesiredSize(float) const 
{
	return Size; 
}


void SDaylonParticles::SetSize                  (const FVector2D& InSize)    { Size = InSize; }
void SDaylonParticles::SetParticleBrush         (const FSlateBrush& InBrush) { ParticleBrush = InBrush; }
void SDaylonParticles::SetMinParticleSize       (float InSize)               { MinParticleSize = InSize; }
void SDaylonParticles::SetMaxParticleSize       (float InSize)               { MaxParticleSize = InSize; }
void SDaylonParticles::SetMinParticleVelocity   (float Velocity)             { MinParticleVelocity = Velocity; }
void SDaylonParticles::SetMaxParticleVelocity   (float Velocity)             { MaxParticleVelocity = Velocity; }
void SDaylonParticles::SetMinParticleLifetime   (float Lifetime)             { MinParticleLifetime = Lifetime; }
void SDaylonParticles::SetMaxParticleLifetime   (float Lifetime)             { MaxParticleLifetime = Lifetime; }
void SDaylonParticles::SetFinalOpacity          (float Opacity)              { FinalOpacity = Opacity; }


void SDaylonParticles::SetNumParticles          (int32 Count)             
{
	check(Count > 0);
	Particles.SetNum(Count);
	Reset();
}


void SDaylonParticles::Reset()
{
	for(auto& Particle : Particles)
	{
		Particle.P             = FVector2D(0); // todo: could randomize this a small distance for more realism
		Particle.Size          = Daylon::FRandRange(MinParticleSize, MaxParticleSize);
		Particle.Inertia       = Daylon::RandVector2D() * Daylon::FRandRange(MinParticleVelocity, MaxParticleVelocity);
		Particle.LifeRemaining = Particle.StartingLifeRemaining = Daylon::FRandRange(MinParticleLifetime, MaxParticleLifetime);
	}
}


bool SDaylonParticles::Update(float DeltaTime)
{
	bool Alive = false;

	for(auto& Particle : Particles)
	{
		Alive |= (Particle.Update(DeltaTime)); 
	}

	return Alive;
}


int32 SDaylonParticles::OnPaint
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
	if(!IsValid(ParticleBrush.GetResourceObject()))
	{
		return LayerId;
	}

	for(const auto& Particle : Particles)
	{
		if(Particle.LifeRemaining <= 0.0f)
		{
			continue;
		}

		// Draw the particle.
		
		const FPaintGeometry PaintGeometry(
			AllottedGeometry.GetAbsolutePosition() + AllottedGeometry.GetAbsoluteSize() / 2  + (Particle.P * AllottedGeometry.Scale) + 0.5f, 
			FVector2D(Particle.Size) * AllottedGeometry.Scale,
			1.0f);

		// Overdrive starting opacity so that we don't start darkening right away.
		float CurrentOpacity = FMath::Lerp(FinalOpacity, 1.5f, Particle.LifeRemaining / Particle.StartingLifeRemaining);
		CurrentOpacity = FMath::Min(1.0f, CurrentOpacity);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			PaintGeometry,
			&ParticleBrush,
			ESlateDrawEffect::None,
			ParticleBrush.TintColor.GetSpecifiedColor() * RenderOpacity * CurrentOpacity * InWidgetStyle.GetColorAndOpacityTint().A);
	}

	return LayerId;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
