// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CReformerPart : public CEnemy
	{
		public:
			CReformerPart();
			virtual ~CReformerPart();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual float GetExplosionMass() const override { return 0.25f; }
			void Explode(CGameObjectCollection&);
			


			void SetOriginalPosition(const CFPoint& pt)
				{ m_posOrg = pt; }

		private:
			float	m_yoff;
			float	m_freq;
			float	m_xFreq;
			float	m_amp;
			float	m_halfwayAltitude;
			float	m_fTimeTargetWithinRange;
			CFPoint	m_posOrg;
			float	m_fMergeTime;
	};


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

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move          (float);
			virtual void Draw          (FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawPart      (FPaintArguments&, const CFPoint&);
			virtual void OnAboutToDie  ();
			void         Explode       (CGameObjectCollection&);
			
			bool         Fireballs     () const { return false; }

		protected:
			float   m_yoff;
			float   m_freq;
			float   m_xFreq;
			float   m_yOrg;

			size_t	m_numParts;
			bool	m_bWaits;
			float	m_fSpinVel;
			float	m_fSpinAngle;
			float	m_fSpinVelMax;
			CFPoint	m_partLocs[10];

	};
}

