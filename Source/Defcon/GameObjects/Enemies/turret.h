// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CTurret : public CEnemy
	{
		// A turret is a stationary gun on the ground that shoots antiaircraft fire.

		typedef CEnemy Super;

		public:
			CTurret();
			virtual ~CTurret();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move					(float) override;
			virtual void Draw					(FPaintArguments&, const I2DCoordMapper&) override;
			virtual float GetExplosionMass		() const override { return 1.25f; }
			virtual EColor GetExplosionColorBase  () const override;


			virtual bool Fireballs		() const override { return false; }
			virtual void Explode		(CGameObjectCollection&) override;

			void SetOriginalPosition	(const CFPoint& pt)		{ m_posOrg = pt; }


		private:

			CFPoint	m_posOrg;
			float	m_freq;
			float	m_fTimeTargetWithinRange;
	};
}
