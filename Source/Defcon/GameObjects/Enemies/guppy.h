// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "enemies.h"
#include "Globals/prefs.h"


namespace Defcon
{
	class CGuppy : public CEnemy
	{
		public:
			CGuppy();

			virtual void   Tick                   (float DeltaTime) override;
			virtual bool   ExplosionHasFireball   () const override { return false; }
			virtual void   Explode                (CGameObjectCollection&) override;
			virtual float  GetExplosionMass       () const override;
			virtual EColor GetExplosionColorBase  () const override;
			virtual float  GetCollisionForce      () const override { return 0.01f * GUPPY_COLLISION_FORCE; }


		protected:
			
			void  ConsiderFiringBullet();


		private:
			
			enum EState { Lounging, Fighting, Evading };

			CFPoint  TargetOffset;
			float    TimeTargetWithinRange;
			float    Frequency;
			float    Amplitude;
			float    FiringCountdown = 0.0f;
			EState   State;
	};
}
