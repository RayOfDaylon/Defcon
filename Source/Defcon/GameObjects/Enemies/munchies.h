// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"
#include "Globals/_sound.h"


namespace Defcon
{
	class IMunchie : public CEnemy
	{
		// Base class of munchie enemies -- munchie, phred, and bigred.
		// Motion is done via ILiveGameObject physics.

		public:
			IMunchie();
			virtual ~IMunchie() {}

			virtual void   Move                   (float DeltaTime) override;


		protected:

			float	     SquakTime;
			EAudioTrack  AudioTrack;
			bool	     bPreferTargetUnderside;
	};


	class CPhred : public IMunchie
	{
		public:
			CPhred();
			virtual ~CPhred() {}

#ifdef _DEBUG
			virtual const char* GetClassname() const { static char* psz = "Phred"; return psz; }
#endif
			virtual EColor GetExplosionColorBase  () const override { return EColor::Gray; }
	};


	class CBigRed : public IMunchie
	{
		public:
			CBigRed();
			virtual ~CBigRed() {}

#ifdef _DEBUG
			virtual const char* GetClassname() const { static char* psz = "BigRed"; return psz; }
#endif
			virtual EColor GetExplosionColorBase  () const override { return EColor::Red; }
	};


	class CMunchie : public IMunchie
	{
		public:
			CMunchie();
			virtual ~CMunchie() {}

#ifdef _DEBUG
			virtual const char* GetClassname() const { static char* psz = "Munchie"; return psz; }
#endif
			virtual EColor GetExplosionColorBase () const override { return EColor::Green; }
			virtual float  GetExplosionMass      () const override { return 0.6f; }
	};
}
