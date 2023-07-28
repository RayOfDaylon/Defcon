// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "UDaylonStarfield.h"


UDaylonStarfield::UDaylonStarfield(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsVariable = false;
}


TSharedRef<SWidget> UDaylonStarfield::RebuildWidget()
{
	MyStarfield = SNew(SDaylonStarfield);

	return MyStarfield.ToSharedRef();
}


void UDaylonStarfield::SetColors(const TArray<FLinearColor>& InColors)
{
	MyStarfield->SetColors(InColors);
}


void UDaylonStarfield::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyStarfield->SetCount(Count);
	MyStarfield->SetSize(Size);
	MyStarfield->SetBrush(Brush);
	MyStarfield->SetSpeed(Speed);
	MyStarfield->SetRotationSpeed(RotationSpeed);

	MyStarfield->SetAge(Age);
	MyStarfield->SetFadeStartsAt(FadeStartsAt);
	MyStarfield->SetFadeEndsAt(FadeEndsAt);
	MyStarfield->SetOsStart(OsStart);
	MyStarfield->SetOsEnd(OsEnd);

	MyStarfield->Reset();
}


void UDaylonStarfield::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyStarfield.Reset();
}



void UDaylonStarfield::Tick(float DeltaTime)
{
	check(MyStarfield);
	MyStarfield->Update(DeltaTime);
}
