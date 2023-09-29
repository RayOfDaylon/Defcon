// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/Painter.h"
#include "Common/util_geom.h"
#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"

/*
	Powerups are non-hostile objects that, when collided with by the player ship, 
	disappear and add useful stuff to it.
*/

namespace Defcon
{
	class IPowerup : public IGameObject
	{
		typedef IGameObject Super;

		public:
			IPowerup();
			virtual void Tick         (float DeltaTime) override;
			virtual void SetSpeed     (float f) { Speed = f; }

		protected:
			float		    Speed = 0.0f; // todo: not sure if we'll need this
	};


	class CShieldPowerup : public IPowerup
	{
		// Player ship shields get an immediate regenerative boost.

		typedef IPowerup Super;

		public:
			CShieldPowerup();

	};


	class CDoubleGunsPowerup : public IPowerup
	{
		// Player ship fires two rounds per shot, for a limited time.
		// Useful when e.g. trying to hit dynamos protected by numerous spacehums.

		typedef IPowerup Super;

		public:
			CDoubleGunsPowerup();

	}; 


	class CInvincibilityPowerup : public IPowerup
	{
		// Player ship becomes invincible for a limited time.
		// Handy if e.g. one needs to collide with pods which normally
		// will take down all shields immediately.

		typedef IPowerup Super;

		public:
			CInvincibilityPowerup();

	}; 
}
