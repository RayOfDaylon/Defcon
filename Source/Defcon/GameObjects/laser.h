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
		// The beams that the player fires.
		// The leading position is the object's position.
		// But we also maintain a trailing position.
		public:
			CLaserbeam();

			virtual void  Move              (float DeltaTime) override;
			virtual void  Draw              (FPainter&, const I2DCoordMapper&) override;
			virtual void  DrawSmall         (FPainter&, const I2DCoordMapper&, FSlateBrush&) override;

			virtual void  GetInjurePt       (CFPoint&) const override;
			virtual bool  TestInjury        (const CFRect&) const override;

			virtual bool  OccursFrequently  () const override { return true; }
			virtual float GetCollisionForce () const override { return 1.1f; }
			
			void          Create            (const CFPoint&, const Orient2D&);
			void          SetMaxLength      (float);


		protected:
			CFPoint       StartPosition;
			float         Length;
			float         Scale;
			float         MaxAge;
			float         MaxLength = BEAM_MAXLEN; // px
	};
}
