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
			void SetFlightDuration(float t) { m_fMaxAge = t; }

		protected:
			CBezierSpline2D	m_path;

			float			m_fMaxAge;

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
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawPart(FPaintArguments&, const CFPoint&);
			virtual void OnAboutToDie();
			void Explode(CGameObjectCollection&);
			
			bool Fireballs() const { return true; }

		protected:
			float m_yoff	= 0.0f;
			float m_freq	= 2.0f;
			float m_xFreq	= 1.0f;
			float m_yOrg;

			int32	m_numParts;
			bool	m_bWaits;
			float	m_fSpinVel;
			float	m_fSpinAngle;
			float	m_fSpinVelMax;
			CFPoint	m_partLocs[10];

			float    DispersalCountdown = 0.0f;
	};
}
