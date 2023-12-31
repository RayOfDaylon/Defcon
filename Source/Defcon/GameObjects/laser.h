// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// laser.h



#include "Common/Painter.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/weapon.h"

#define BEAM_MAXLEN     700


namespace Defcon
{
	class CLaserWeapon : public CWeapon
	{
		// A laser weapon is used by the player 
		// to shoot fast thin tracers at enemies.

		public:

			CLaserWeapon()
			{
				m_maxWeaponBursts = 10;
			}

			virtual void Fire(CGameObjectCollection&);
	};


	class CLaserbeam : public IGameObject
	{
		// The beams that the player ship fires.

		typedef IGameObject Super;

		public:

			CLaserbeam();

			void          InitLaserBeam     (const CFPoint&, const Orient2D&, I2DCoordMapper*);

			virtual void  Tick              (float DeltaTime) override;
			virtual void  DrawSmall         (FPainter&, const Defcon::I2DCoordMapper&, FSlateBrush&) const override {}

			virtual void  GetInjurePt       (CFPoint&) const override;
			virtual bool  TestInjury        (const CFRect&) const override;

			virtual bool  OccursFrequently  () const override { return true; }
			virtual float GetCollisionForce () const override { return 1.1f; }


		protected:

			float  StartX;
			float  EndX;
			float  EstimatedLifetime;
	};
}
