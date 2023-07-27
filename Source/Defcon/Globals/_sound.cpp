/*
	_sound.cpp
	Sound stuff.
	Copyright 2004 Daylon Graphics Ltd.
*/



#include "_sound.h"

#include "prefs.h"
#include "Common/util_core.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DefconUtils.h"


Defcon::CAudioManager* gpAudio = nullptr;



Defcon::CAudioManager::CAudioManager(UObject* InWorldContextObject)
{
	check(InWorldContextObject != nullptr);

	WorldContextObject = InWorldContextObject;

	if(!this->CreateSoundObjects())
	{
#ifdef _DEBUG
		OutputDebugString("\n*** ERROR: Sound objects not created\n");
#endif
	}
}


Defcon::CAudioManager::~CAudioManager()
{
    this->DestroySoundObjects();
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
