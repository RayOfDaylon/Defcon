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

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void  Move                  (float) override;
			virtual void  Draw                  (FPaintArguments&, const I2DCoordMapper&) override;
			virtual void  OnAboutToDie          () override;
			virtual bool  Fireballs             () const override { return false; }
			virtual void  Explode               (CGameObjectCollection&) override;
			virtual float GetExplosionMass      () const override;
			virtual EColor GetExplosionColorBase  () const override;

			float					TimeTargetWithinRange;
			CFPoint					TargetOffset;

		protected:
			void  ConsiderFiringBullet();

		private:
			enum EState { lounging, fighting, evading };
			EState					State;
			float					Frequency;
			float					Amplitude;
			float                   FiringCountdown = 0.0f;
	};
}
