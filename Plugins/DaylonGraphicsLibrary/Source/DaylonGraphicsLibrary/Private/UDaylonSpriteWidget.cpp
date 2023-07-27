// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "UDaylonSpriteWidget.h"


UDaylonSpriteWidget::UDaylonSpriteWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsVariable = false;
}


TSharedRef<SWidget> UDaylonSpriteWidget::RebuildWidget()
{
	MySprite = SNew(SDaylonSprite);

	return MySprite.ToSharedRef();
}


void UDaylonSpriteWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MySprite->SetSize (Size);
	MySprite->SetAtlas(TextureAtlas->Atlas);

	Reset();
}


void UDaylonSpriteWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MySprite.Reset();
}


void UDaylonSpriteWidget::Tick(float DeltaTime)
{
	check(MySprite);
	MySprite->Update(DeltaTime);
}


void UDaylonSpriteWidget::Reset()
{
	check(MySprite);
	MySprite->Reset();
}


