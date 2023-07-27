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
		snd_invalid_selection,
		snd_focus_changed,
		snd_player_dying,
		snd_ship_exploding,
		snd_ship_exploding_small,
		snd_ship_exploding_medium,
		snd_ship_exploding2,			// not used
		snd_ship_exploding2a,			// not used
		snd_ship_exploding2b,			// not used
		snd_ship_exploding_small2,		// not used
		snd_smartbomb,
		snd_laserfire, 
		snd_laserfire_alt,
		snd_mission_chosen,
		snd_wave_start, 
		snd_wave_end,
		snd_ship_materialize,
		snd_human_abducted,
		snd_extra_ship,
		snd_shieldbonk,
		snd_swarmer,
		snd_bullet,
		snd_bullet2,
		snd_bullet3,
		snd_select,
		snd_ghostflight,
		snd_phred, //munchie,
		snd_munchie, // munchie2
		snd_bigred, // munchie3
		//song_main,
		snd_warp,
		snd_playership_thrust,
		snd_gulp,

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

			void AddTrack(EAudioTrack Track, USoundBase* Asset)
			{
				if(!Tracks.Find(Track))
				{
					Tracks.Add(Track, Asset);
				}
			}

			USoundBase* GetTrack(EAudioTrack Track)
			{
				auto p = Tracks.Find(Track);
				if(p == nullptr)
				{
					return nullptr;
				}

				return *p;
			}

		    bool CreateSoundObjects();
			void DestroySoundObjects();


			virtual void OutputSound(EAudioTrack Track, float VolumeScale = 1.0f);
			virtual void StopPlayingSound();
			Daylon::FLoopedSound CreateLoopedSound(EAudioTrack Track, float VolumeScale = 1.0f);


		private:

			TMap<EAudioTrack, USoundBase*> Tracks;
			UObject*                       WorldContextObject = nullptr;
			
			bool IsSoundPlaying() const;
	};
}

extern Defcon::CAudioManager*	gpAudio;

