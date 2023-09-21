// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CGhost : public CEnemy
	{
		// A ghost is a multipart entity that, when threatened,
		// breaks up and reforms a short distance away.
		// The behavior makes it hard to hit. The parts are 
		// invulnerable to weapons fire while in transit.

		typedef CEnemy Super;

		public:
			CGhost();
			virtual ~CGhost();

			virtual void Tick       (float DeltaTime) override;
			virtual void Draw       (FPainter&, const I2DCoordMapper&) override;
			virtual void Explode    (CGameObjectCollection&) override;

			virtual bool ExplosionHasFireball  () const override { return true; }

		protected:

			virtual void DrawPart              (FPainter&, const CFPoint&);
			void         ConsiderFiringBullet  (float DeltaTime);


			CFPoint	              PartLocs[10];
			Daylon::FRange<float> PartRadii[10];
			float                 PartRadiiSpeed[10];
			float                 OffsetY             = 0.0f;
			float                 Frequency           = 2.0f;
			float	              SpinVelocity;
			float	              SpinAngle;
			float                 SpinDir;
			float                 FiringCountdown;
			float                 DispersalCountdown  = 0.0f;
			int32	              NumParts;
	};


	class CGhostPart : public CEnemy
	{
		// Ghost parts are created when a ghost decides 
		// to transport elsewhere. It is given a path to 
		// fly and a length of time to be in flight.

		typedef CEnemy Super;

		public:
			CGhostPart();
			virtual ~CGhostPart();

			virtual void OnFinishedCreating (const FMetadata& Options) override;
			virtual void Tick               (float DeltaTime) override;
			virtual void Draw               (FPainter&, const I2DCoordMapper&) override;
			virtual void Explode            (CGameObjectCollection&) override;
			

			void SetFlightPath              (const CFPoint&, const CFPoint&);
			void SetFlightDuration          (float t) { MaxAge = t; }

		protected:
			CBezierSpline2D	Path;

			float			MaxAge;

	};

}
