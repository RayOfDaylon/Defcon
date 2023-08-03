// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/Painter.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/weapon.h"



namespace Defcon
{
	class CMine : public IGameObject
	{
		// Stationary collision hazards laid by bombers.

		public:
			CMine();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			virtual void  Move               (float DeltaTime) override;
			virtual void  DrawSmall          (FPainter&, const I2DCoordMapper&, FSlateBrush&) override {}

			virtual void  GetInjurePt        (CFPoint&) const override;
			virtual bool  TestInjury         (const CFRect&) const override;
			virtual float GetCollisionForce  () const override { return 0.01f * MINE_DAMAGE; }
	};
}
