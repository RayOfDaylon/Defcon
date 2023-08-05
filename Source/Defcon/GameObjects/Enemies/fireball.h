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

			virtual void    Move                   (float DeltaTime) override;
			virtual EColor  GetExplosionColorBase  () const override;
			virtual float   GetExplosionMass       () const override;
			virtual bool    ExplosionHasFireball   () const  override { return false; }
			void            Explode                (CGameObjectCollection&) override;
			virtual float   GetCollisionForce      () const override { return 0.02f * COLLISION_DAMAGE; }

			float	Speed = 0.0f;
	};
}

