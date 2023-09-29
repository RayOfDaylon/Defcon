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
	enum class EAudioTrack 
	{
		Invalid_selection,
		Focus_changed,
		Player_dying,
		Ship_exploding,
		Ship_exploding_small,
		Ship_exploding_medium,
		Ship_exploding2,			
		Ship_exploding2a,			
		Ship_exploding2b,			
		Ship_exploding_small2,		// not used
		Smartbomb,
		Laserfire, 
		Laserfire_alt,
		Mission_chosen,
		Wave_start, 
		Wave_end,
		Ship_materialize,
		Human_abducted,
		Extra_ship,
		Shieldbonk,
		Swarmer,
		Bullet,
		Bullet2,
		Bullet3,
		Select,
		Ghostflight,
		Phred, //munchie,
		Munchie, // munchie2
		BigRed, // munchie3
		Warp,
		Playership_thrust,
		Gulp,

		NumSounds
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

	extern CAudioManager*	GAudio;  // todo: retire when mediator handles AudioManager public API
}

