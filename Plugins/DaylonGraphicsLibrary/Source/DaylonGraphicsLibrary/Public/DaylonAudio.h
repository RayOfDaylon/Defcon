// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"


namespace Daylon
{
	struct DAYLONGRAPHICSLIBRARY_API FLoopedSound
	{
		// A hacked sound loop created by simply playing the same sound
		// over and over if Tick() is called.

		USoundBase* Sound = nullptr;

		float VolumeScale  = 1.0f;
		float VolumeScale2 = 1.0f;

		UObject* WorldContextPtr = nullptr;

		void Set(UObject* Context, USoundBase* InSound, float InVolumeScale = 1.0f)
		{
			WorldContextPtr = Context;
			Sound           = InSound;
			VolumeScale     = InVolumeScale;
		}

		void Start(float InVolumeScale = 1.0f);

		void Tick(float DeltaTime);

		protected:

			float TimeRemaining = 0.0f;
	};
}
