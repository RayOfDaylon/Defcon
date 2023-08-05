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
			
			virtual void  Move                  (float DeltaTime) override;
			virtual float GetExplosionMass      () const override { return 0.1f; }
			bool          ExplosionHasFireball  () const override { return false; }
			void          Explode               (CGameObjectCollection&) override;
			

		private:
			float	Speed;
	};
}
