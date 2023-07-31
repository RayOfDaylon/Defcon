// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// player.h

#include "CoreMinimal.h"

#include "Common/PaintArguments.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/laser.h"
#include "GameObjects/obj_types.h"
//#include "Globals/GameColors.h"

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

			virtual void Move         (float) override;
			virtual void Draw         (FPaintArguments&, const I2DCoordMapper&) override;
			virtual void DrawSmall    (FPaintArguments&, const I2DCoordMapper&, FSlateBrush& Brush) override;
			virtual void Explode      (CGameObjectCollection&) override;
			virtual void OnAboutToDie () override;
			virtual void SetIsAlive   (bool b) override;

			// For the player ship, we need to customize vertical motion.
			virtual void ImpartForces			(float) override;



#ifdef SHOW_STATS
			void DrawStats(FPaintArguments&, int, int);
#endif

			void FaceLeft   () { Orientation.fwd.x = -1.0f; }
			void FaceRight  () { Orientation.fwd.x = 1.0f; }

			void FireLaserWeapon     (CGameObjectCollection&);
			bool IsSolid             () const;
			bool EmbarkPassenger     (IGameObject*, CGameObjectCollection&);
			bool DebarkOnePassenger  (CGameObjectCollection&);
			const CFPoint& GetPickupRadBox() const { return m_bboxradPickup; }

			virtual bool IsMaterializing() const { return false; }// (m_bMaterializes && Age < PLAYER_BIRTHDURATION); }

			FSlateBrush     RadarBrush;

		private:
			CLaserWeapon	m_laserWeapon;
			//CFPoint			m_birthDebrisLocs[400];
			//CFPoint			m_birthDebrisLocsOrg[400];
			//float			m_debrisPow[400];
			//bool			m_bBirthsoundPlayed;
			CFPoint         m_bboxradPickup;

	}; // CPlayer

}


