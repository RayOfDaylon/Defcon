// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "enemies.h"


namespace Defcon
{
	class CGuppy : public CEnemy
	{
		public:
			CGuppy();

			virtual void   Move                   (float DeltaTime) override;
			virtual void   OnAboutToDie           () override;
			virtual bool   ExplosionHasFireball   () const override { return false; }
			virtual void   Explode                (CGameObjectCollection&) override;
			virtual float  GetExplosionMass       () const override;
			virtual EColor GetExplosionColorBase  () const override;


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
