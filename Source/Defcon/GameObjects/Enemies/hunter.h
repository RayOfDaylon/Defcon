// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "enemies.h"
#include "Globals/prefs.h"


namespace Defcon
{
	class CHunter : public CEnemy
	{
		public:
			CHunter();
			virtual ~CHunter();

			virtual void   Tick                   (float DeltaTime) override;
			virtual float  GetCollisionForce      () const override { return 0.01f * HUNTER_COLLISION_FORCE; }
			virtual EColor GetExplosionColorBase  () const override;
			virtual float  GetExplosionMass       () const override;


		protected:
			
			void  ConsiderFiringBullet();


		private:

			enum EState { Lounging, Fighting, Evading };

			CFPoint     TargetOffset;
			float       TimeTargetWithinRange;
			float       Frequency;
			float       Amplitude;
			float       FiringCountdown = 1.0f;
			EState      State;
	};
}
