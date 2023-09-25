// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// playership.h

#include "CoreMinimal.h"

#include "Globals/MessageMediator.h"
#include "Common/Painter.h"
#include "Common/util_geom.h"
#include "Common/ParticleGroup.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/laser.h"
#include "GameObjects/obj_types.h"

#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"


class UDefconPlayerShipDebugWidgetBase;


namespace Defcon
{
	class CPlayerShip : public ILiveGameObject
	{
		friend class ::UDefconPlayerShipDebugWidgetBase;

		typedef ILiveGameObject Super;

		public:

			CPlayerShip();
			virtual ~CPlayerShip();

			void InitPlayerShip();

			virtual void   Tick                    (float DeltaTime) override;
			virtual void   DrawSmall               (FPainter&, const I2DCoordMapper&, FSlateBrush& Brush) const override;
			virtual void   OnAboutToDie            () override;
			virtual void   SetIsAlive              (bool b) override;
			virtual void   ImpartForces            (float) override; // For the player ship, we need to customize vertical motion.
			virtual bool   CanBeInjuredBySmartbomb (int32 BombID) const override { return false; }

			void           FaceLeft                () { Orientation.Fwd.x = -1.0f; }
			void           FaceRight               () { Orientation.Fwd.x = 1.0f; }
				           					       
			bool           AcquireSmartBomb        ();
			void           AddSmartBombs           (int32 Amount);
				           
			void           FireLaserWeapon         (CGameObjectCollection&);
			bool           AreDoubleGunsActive     () const { return DoubleGunsActive; }
			void           ToggleDoubleGuns        () { DoubleGunsActive = !DoubleGunsActive; }
			void           DeactivateDoubleGuns    () { DoubleGunsActive = false; }

			bool           EmbarkPassenger         (IGameObject*, CGameObjectCollection&);
			bool           DebarkOnePassenger      (CGameObjectCollection&);
			const CFPoint& GetPickupRadiusBox      () const { return PickupBboxRadius; }


			FSlateBrush     RadarBrush;


		private:

			TMessageableValue<int32>  SmartbombsLeft;
			CLaserWeapon              LaserWeapon;
			CLaserWeapon              SecondaryLaserWeapon;
			CFPoint                   PickupBboxRadius;
			bool                      DoubleGunsActive = false;

	};


	class CDestroyedPlayerShip : public Defcon::ILiveGameObject
	{
		private:

			FParticleGroup    ParticleGroups[100];

			// We can update and draw a subset of the particle groups every frame,
			// to mimic the arcade game better.
			int32             NumParticleGroupsToUpdate = 1;
			int32             NumParticleGroupsToDraw = array_size(ParticleGroups);

		public:

			CDestroyedPlayerShip();

			void InitDestroyedPlayer(const CFPoint& ShipP, const CFPoint& ShipS, float ParticleSpeed, float MinParticleLifetime, float MaxParticleLifetime);

			virtual void  Tick  (float DeltaTime) override;
			virtual void  Draw  (FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper) override;
	};
}
