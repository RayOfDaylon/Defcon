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
			virtual void  Move              (float DeltaTime) override;
			virtual void  Draw              (FPaintArguments&, const I2DCoordMapper&) override;
			virtual float GetExplosionMass  () const override { return 0.1f; }

			bool          Fireballs         () const override { return false; }
			void          Explode           (CGameObjectCollection&) override;
			

		private:
			float	Speed;
	};
}
