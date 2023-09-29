// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	_sound.cpp
*/


#include "_sound.h"

#include "prefs.h"
#include "MessageMediator.h"
#include "Common/util_core.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DefconUtils.h"


Defcon::CAudioManager* Defcon::GAudio = nullptr;



Defcon::CAudioManager::CAudioManager(UObject* InWorldContextObject)
{
	check(InWorldContextObject != nullptr);

	WorldContextObject = InWorldContextObject;

	if(!CreateSoundObjects())
	{
#ifdef _DEBUG
		OutputDebugString("\n*** ERROR: Sound objects not created\n");
#endif
	}

	FMessageConsumer MessageConsumer(this, EMessageEx::PlaySound, [this](void* Payload)
	{
		check(Payload != nullptr);

		const auto& Params = *static_cast<FPlaySoundMessage*>(Payload);
		OutputSound(Params.Track, Params.Volume);
	});

	GMessageMediator.RegisterConsumer(MessageConsumer);
}


Defcon::CAudioManager::~CAudioManager()
{
    DestroySoundObjects();
}


Daylon::FLoopedSound Defcon::CAudioManager::CreateLoopedSound(EAudioTrack Track, float VolumeScale)
{
	Daylon::FLoopedSound LoopedSound;
	LoopedSound.Set(WorldContextObject, GetTrack(Track), VolumeScale);
	return LoopedSound;
}


void Defcon::CAudioManager::OutputSound(EAudioTrack e, float VolumeScale)
{
	// Emit a sound. All sound output is done here.

	auto TrackPtr = GetTrack(e);

	if(TrackPtr != nullptr)
	{
		UGameplayStatics::PlaySound2D(WorldContextObject, TrackPtr, VolumeScale);
	}
}


bool Defcon::CAudioManager::CreateSoundObjects()
{
#ifdef _DEBUG
	OutputDebugString("Creating sound objects\n");
#endif

    return true;
}


void Defcon::CAudioManager::DestroySoundObjects()
{
#ifdef _DEBUG
	OutputDebugString("Destroying sound objects\n");
#endif
}


bool Defcon::CAudioManager::IsSoundPlaying() const
{
	return false;
}


void Defcon::CAudioManager::StopPlayingSound()
{
}


void Defcon::CAudioManager::AddTrack(EAudioTrack Track, USoundBase* Asset)
{
	if(!Tracks.Find(Track))
	{
		Tracks.Add(Track, Asset);
	}
}


USoundBase* Defcon::CAudioManager::GetTrack(EAudioTrack Track)
{
	auto Sound = Tracks.Find(Track);

	if(Sound == nullptr)
	{
		return nullptr;
	}

	return *Sound;
}
