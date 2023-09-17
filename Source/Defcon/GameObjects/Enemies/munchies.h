// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"


namespace Defcon
{
	class IMunchie : public CEnemy
	{
		// Base class of munchie enemies -- munchie, phred, and bigred.
		// Motion is done via ILiveGameObject physics.

		typedef CEnemy Super;

		public:
			IMunchie();
			virtual ~IMunchie() {}

			virtual void   Tick   (float DeltaTime) override;


		protected:

			float	     SquakTime;
			EAudioTrack  AudioTrack;
			bool	     bPreferTargetUnderside;
	};


	class CPhred : public IMunchie
	{
		typedef IMunchie Super;

		public:
			CPhred();
			virtual ~CPhred() {}

			virtual void   Tick                   (float DeltaTime) override;
			virtual EColor GetExplosionColorBase  () const override { return EColor::Gray; }


		protected:

			float MunchieSpawnCountdown;
	};


	class CBigRed : public IMunchie
	{
		typedef IMunchie Super;

		public:
			CBigRed();
			virtual ~CBigRed() {}

			virtual EColor GetExplosionColorBase  () const override { return EColor::Red; }
	};


	class CMunchie : public IMunchie
	{
		typedef IMunchie Super;

		public:
			CMunchie();
			virtual ~CMunchie() {}

			virtual EColor GetExplosionColorBase () const override { return ColorCode; }
			virtual float  GetExplosionMass      () const override { return 0.6f; }
			virtual float  GetCollisionForce     () const override { return 0.01f * MUNCHIE_COLLISION_FORCE; }


		protected:
			
			EColor ColorCode;
	};
}
