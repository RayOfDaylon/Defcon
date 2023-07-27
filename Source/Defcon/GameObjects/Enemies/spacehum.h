// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"



namespace Defcon
{
	class CSpacehum : public CEnemy
	{
		// A space hum floats around but hunts player. 

		public:
			CSpacehum();
			
#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual float GetExplosionMass() const { return 0.1f; }

			bool Fireballs() const { return false; }
			void Explode(CGameObjectCollection&);
			

		private:
			float	m_fSpeed;
	};
}

