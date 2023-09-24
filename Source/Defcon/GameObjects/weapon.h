// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// weapon.h

#include "Common/util_geom.h"
#include "gameobj.h"


namespace Defcon
{
	class CWeapon
	{
		// A weapon is something that gives an entity 
		// some offensive firepower. This base class 
		// needs to be subclassed for specific weapons.

		public:
			CWeapon()
				:
				m_maxWeaponBursts(1),
				m_numShotsAlive(0),
				Wielder(nullptr)
			{
			}

			virtual ~CWeapon() {}

			virtual void Fire(CGameObjectCollection&) {}

			void MountOnto(IGameObject& obj, const CFPoint& pt)
				{
					Wielder = &obj; 
					m_mountPt = pt; 
				}

			void SetEmissionPt(const CFPoint& pt)
				{
					m_emissionPt = pt;
				}



		protected:
			// Where the weapon is mounted on the object, 
			// relative to the object.
			CFPoint	m_mountPt;

			// Where the weapon emits its fire, relative to the object.
			CFPoint m_emissionPt;

			// If m_numShotsAlive equals m_maxWeaponBursts, 
			// then the object is not allowed to fire.
			// This prevents objects from just filling 
			// space with unending weapons fire.
			int32		m_maxWeaponBursts;
			int32		m_numShotsAlive;

			// The object that the weapon is mounted on.
			// We need this reference so that we can
			// query the object for its size and location.
			IGameObject*	Wielder;
	};  // CWeapon
}
