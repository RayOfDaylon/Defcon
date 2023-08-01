// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CGhostPart : public CEnemy
	{
		// Ghost parts are created when a ghost decides 
		// to transport elsewhere. It is given a path to 
		// fly and a length of time to be in flight.

		public:
			CGhostPart();
			virtual ~CGhostPart();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			void Explode(CGameObjectCollection&);
			

			void SetFlightPath(const CFPoint&, const CFPoint&);
			void SetFlightDuration(float t) { MaxAge = t; }

		protected:
			CBezierSpline2D	Path;

			float			MaxAge;

	};


	class CGhost : public CEnemy
	{
		// A ghost is a multipart entity that, when threatened,
		// breaks up and reforms a short distance away.
		// The behavior makes it hard to hit. The parts are 
		// invulnerable to weapons fire while in transit.


		public:
			CGhost();
			virtual ~CGhost();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move       (float DeltaTime) override;
			virtual void Draw       (FPaintArguments&, const I2DCoordMapper&) override;
			virtual void Explode    (CGameObjectCollection&) override;

			virtual bool Fireballs  () const override { return true; }

		protected:

			virtual void DrawPart(FPaintArguments&, const CFPoint&);

			CFPoint	 PartLocs[10];
			float    OffsetY	= 0.0f;
			float    Frequency	= 2.0f;
			float	 SpinVelocity;
			float	 SpinAngle;
			int32	 NumParts;
			bool	 bWaits;

			float    DispersalCountdown = 0.0f;
	};
}
