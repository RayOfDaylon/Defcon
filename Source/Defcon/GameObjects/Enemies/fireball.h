// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CFireball : public CEnemy
	{
		public:
			CFireball();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual EColor GetExplosionColorBase  () const override;
			virtual float GetExplosionMass() const;
			virtual bool Fireballs() const { return false; }
			void Explode(CGameObjectCollection&);
			

			virtual float GetCollisionForce() const override { return 0.02f * COLLISION_DAMAGE; }

			float	m_fSpeed;

		private:
			bool	m_bFirstTime;	
	};
}

