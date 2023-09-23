// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DaylonAudio.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"



void Daylon::FLoopedSound::Start(float InVolumeScale)
{
	VolumeScale2 = InVolumeScale;
	TimeRemaining = 0.0f;
	Tick(0.0f);
}


void Daylon::FLoopedSound::Tick(float DeltaTime)
{
	TimeRemaining -= DeltaTime;

	if(Sound == nullptr || WorldContextPtr == nullptr)
	{
		//UE_LOG(LogDaylon, Error, TEXT("Looped sound has no sound or context"));
		return;
	}

	if(TimeRemaining <= 0.0f)
	{
		TimeRemaining = Sound->GetDuration();

		UGameplayStatics::PlaySound2D(WorldContextPtr, Sound, VolumeScale * VolumeScale2);
	}
}
