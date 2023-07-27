// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Components/Widget.h"

#include "SDaylonParticles.h"
#include "UDaylonParticlesWidget.generated.h"

// Warning: do NOT use this widget at design time.

UCLASS(meta=(DisplayName="Particles Widget"))
class DAYLONGRAPHICSLIBRARY_API UDaylonParticlesWidget : public UWidget
{
	GENERATED_UCLASS_BODY()

	public:  

		FVector2D   Size                = FVector2D(64);
		float       MinParticleSize     =  3.0f;
		float       MaxParticleSize     =  3.0f;
		float       MinParticleVelocity = 30.0f;
		float       MaxParticleVelocity = 80.0f;
		float       MinParticleLifetime =  0.25f;
		float       MaxParticleLifetime =  1.0f;
		float       FinalOpacity        = 1.0f;
		int32       NumParticles        = 40;
		FSlateBrush ParticleBrush;

		bool Update  (float DeltaTime);
		void Reset   ();

		virtual void SynchronizeProperties () override;
		virtual void ReleaseSlateResources (bool bReleaseChildren) override;


	protected:

		virtual TSharedRef<SWidget> RebuildWidget() override;

		TSharedPtr<SDaylonParticles> MyParticlesWidget;
};
