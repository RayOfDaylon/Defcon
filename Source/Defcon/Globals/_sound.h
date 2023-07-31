// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// _sound.h
// Local sound interfaces.

#include "Common/compat.h"
#include "DaylonUtils.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"


namespace Defcon
{
	enum EAudioTrack 
	{
		invalid_selection,
		focus_changed,
		player_dying,
		ship_exploding,
		ship_exploding_small,
		ship_exploding_medium,
		ship_exploding2,			
		ship_exploding2a,			
		ship_exploding2b,			
		ship_exploding_small2,		// not used
		smartbomb,
		laserfire, 
		laserfire_alt,
		mission_chosen,
		wave_start, 
		wave_end,
		ship_materialize,
		human_abducted,
		extra_ship,
		shieldbonk,
		swarmer,
		bullet,
		bullet2,
		bullet3,
		select,
		ghostflight,
		phred, //munchie,
		munchie, // munchie2
		bigred, // munchie3
		warp,
		playership_thrust,
		gulp,

		numSounds
	};
}



namespace Defcon
{
	class CAudioManager
	{
		public:
			CAudioManager(UObject* InWorldContextObject);
			virtual ~CAudioManager();

			void                 AddTrack(EAudioTrack Track, USoundBase* Asset);
			USoundBase*          GetTrack(EAudioTrack Track);

		    bool                 CreateSoundObjects   ();
			void                 DestroySoundObjects  ();
			virtual void         OutputSound          (EAudioTrack Track, float VolumeScale = 1.0f);
			virtual void         StopPlayingSound     ();
			Daylon::FLoopedSound CreateLoopedSound    (EAudioTrack Track, float VolumeScale = 1.0f);


		private:

			TMap<EAudioTrack, USoundBase*> Tracks;
			UObject*                       WorldContextObject = nullptr;
			
			bool IsSoundPlaying() const;
	};
}

extern Defcon::CAudioManager*	gpAudio;
