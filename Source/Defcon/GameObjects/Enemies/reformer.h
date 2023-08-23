// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CReformer : public CEnemy
	{
		// A reformer is like a pod, except that it can fire, 
		// and if hit, its parts will, after flying outward 
		// from the detonation, attempt to regroup into a 
		// new reformer. Two or more parts can make a 
		// reformer, but the less parts there are the 
		// smaller and less powerful the built reformer 
		// will be. The player can (and should) shoot the 
		// parts before they successfully merge.

		public:
			CReformer();
			virtual ~CReformer();

			virtual void Tick                  (float DeltaTime) override;
			virtual void Draw                  (FPainter&, const I2DCoordMapper&) override;
			virtual void OnAboutToDie          () override;
			virtual void Explode               (CGameObjectCollection&) override;
			virtual bool ExplosionHasFireball  () const override { return false; }
			virtual void DrawPart              (FPainter&, const CFPoint&);


		protected:

			void ConsiderFiringBullet  (float DeltaTime);


		private:
			CFPoint  PartLocations[10];
			float    FiringCountdown;
			float    VerticalOffset;
			float    Frequency;
			float    SpinVelocity;
			float    SpinAngle;
			float    MaxSpinVelocity;
			int32    NumParts;
			//bool     bWaits;
	};


	class CReformerPart : public CEnemy
	{
		public:
			CReformerPart();
			virtual ~CReformerPart();

			virtual void  Tick              (float DeltaTime) override;
			virtual float GetExplosionMass  () const override { return 0.25f; }
			virtual void  Explode           (CGameObjectCollection&) override;
			
			void SetOriginalPosition(const CFPoint& P) { OriginalPos = P; }

		private:
			CFPoint  OriginalPos;
			float    VerticalOffset;
			float    Frequency;
			float    HorzFrequency;
			float    Amplitude;
			float    HalfwayAltitude;
			float    TimeTargetWithinRange;
			float    MergeTime;
	};
}
