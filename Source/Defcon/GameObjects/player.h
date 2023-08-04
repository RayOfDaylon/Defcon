// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// player.h

#include "CoreMinimal.h"

#include "Common/Painter.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/laser.h"
#include "GameObjects/obj_types.h"

#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"


class UDefconPlayerShipDebugWidgetBase;


namespace Defcon
{
	class CPlayer : public ILiveGameObject
	{
		friend class ::UDefconPlayerShipDebugWidgetBase;

		typedef ILiveGameObject Super;

		public:
			CPlayer();
			virtual ~CPlayer();

			void InitPlayer(float);

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			virtual void  Move          (float DeltaTime) override;
			virtual void  DrawSmall     (FPainter&, const I2DCoordMapper&, FSlateBrush& Brush) override;
			virtual void  OnAboutToDie  () override;
			virtual void  SetIsAlive    (bool b) override;
			virtual void  ImpartForces  (float) override; // For the player ship, we need to customize vertical motion.


#ifdef SHOW_STATS
			void DrawStats(FPaintArguments&, int32, int32);
#endif

			void FaceLeft                     () { Orientation.Fwd.x = -1.0f; }
			void FaceRight                    () { Orientation.Fwd.x = 1.0f; }

			void FireLaserWeapon              (CGameObjectCollection&);
			bool EmbarkPassenger              (IGameObject*, CGameObjectCollection&);
			bool DebarkOnePassenger           (CGameObjectCollection&);
			const CFPoint& GetPickupRadiusBox () const { return PickupBboxRadius; }

			FSlateBrush     RadarBrush;


		private:

			CLaserWeapon	LaserWeapon;
			CFPoint         PickupBboxRadius;

	}; // CPlayer
}
