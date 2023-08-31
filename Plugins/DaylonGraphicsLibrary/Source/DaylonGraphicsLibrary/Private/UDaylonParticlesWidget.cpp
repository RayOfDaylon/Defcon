// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "UDaylonParticlesWidget.h"


#if WITH_EDITOR
const FText UDaylonParticlesWidget::GetPaletteCategory()
{
	return FText::FromString(TEXT("Daylon"));
}
#endif


UDaylonParticlesWidget::UDaylonParticlesWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsVariable = false;
}


TSharedRef<SWidget> UDaylonParticlesWidget::RebuildWidget()
{
	MyParticlesWidget = SNew(SDaylonParticles);

	return MyParticlesWidget.ToSharedRef();
}


void UDaylonParticlesWidget::SynchronizeProperties ()
{
	Super::SynchronizeProperties();

	MyParticlesWidget->SetSize                   (Size);
	MyParticlesWidget->SetMinParticleSize        (MinParticleSize);
	MyParticlesWidget->SetMaxParticleSize        (MaxParticleSize);
	MyParticlesWidget->SetMinParticleVelocity    (MinParticleVelocity);
	MyParticlesWidget->SetMaxParticleVelocity    (MaxParticleVelocity);
	MyParticlesWidget->SetMinParticleLifetime    (MinParticleLifetime);
	MyParticlesWidget->SetMaxParticleLifetime    (MaxParticleLifetime);
	MyParticlesWidget->SetNumParticles           (NumParticles);
	MyParticlesWidget->SetFinalOpacity           (FinalOpacity);
	MyParticlesWidget->SetParticleBrush          (ParticleBrush);

	Reset();
}


void UDaylonParticlesWidget::ReleaseSlateResources (bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyParticlesWidget.Reset();
}


bool UDaylonParticlesWidget::Update                   (float DeltaTime)
{
	check(MyParticlesWidget);
	return MyParticlesWidget->Update(DeltaTime);
}


void UDaylonParticlesWidget::Reset                    ()
{
	check(MyParticlesWidget);
	MyParticlesWidget->Reset();
}


