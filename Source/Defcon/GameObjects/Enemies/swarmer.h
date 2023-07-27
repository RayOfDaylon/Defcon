// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CSwarmer : public CEnemy
	{
		// A swarmer is a tiny orange guy that flies 
		// and shoots, and normally erupts from pods.
		public:
			CSwarmer();
			virtual ~CSwarmer();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual float GetExplosionMass() const override { return 0.1f; }
			bool Fireballs() const { return false; }
			void Explode(CGameObjectCollection&);

			void SetOriginalPosition(const CFPoint& pt)	{ m_posOrg = pt; }

		private:
			float m_yoff;
			float m_freq;
			float m_xFreq;
			float m_amp;
			float m_halfwayAltitude;
			float m_fTimeTargetWithinRange;
			float CountdownToMakeSound;
			CFPoint	m_posOrg;
	};
}
