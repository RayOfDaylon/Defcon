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
	class IBullet : public IGameObject
	{
		public:
			IBullet();
			virtual void Tick         (float DeltaTime) override;
			virtual void DrawSmall    (FPainter& PaintArgs, const I2DCoordMapper& CoordMapper, FSlateBrush& Brush) const override {}
			virtual void GetInjurePt  (CFPoint&) const override;
			virtual bool TestInjury   (const CFRect&) const override;
			virtual void SetSpeed     (float f) { Speed = f; }


			virtual float GetCollisionForce() const { return 0.01f * BULLET_DAMAGE; }


		protected:
			FLinearColor    Color;
			float		    Speed;
			int32           NumSpriteCels = 0;
	};


	class CBullet : public IBullet
	{
		// The harmful particles fired by enemies.
		public:
			CBullet();
	};


	class CThinBullet : public IBullet
	{
		// The harmful particles fired by space guppies.
		public:
			CThinBullet();
	}; 
}


