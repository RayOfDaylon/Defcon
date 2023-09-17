// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"
#include "Globals/prefs.h"


namespace Defcon
{
	class CSwarmer : public CEnemy
	{
		// A swarmer is a tiny orange guy that flies and shoots, and normally erupts from pods.

		public:
			CSwarmer();
			virtual ~CSwarmer();

			virtual void  Tick                  (float DeltaTime) override;
			virtual float GetCollisionForce     () const override { return 0.01f * SWARMER_COLLISION_FORCE; }
			virtual float GetExplosionMass      () const override { return 0.1f; }
			virtual bool  ExplosionHasFireball  () const override { return false; }
			virtual void  Explode               (CGameObjectCollection&) override;

			void SetOriginalPosition  (const CFPoint& pt) { OriginalPos = pt; }

		private:

			void ConsiderFiringBullet (float DeltaTime);

			CFPoint	OriginalPos;
			float   FiringCountdown;
			float   VerticalOffset;
			float   Frequency;
			float   HorzFrequency;
			float   Amplitude;
			float   HalfwayAltitude;
			float   TimeTargetWithinRange;
			float   SoundCountdown;
	};
}
