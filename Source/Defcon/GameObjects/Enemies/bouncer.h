// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class IBouncer : public CEnemy
	{
		// A bouncer bounces on the terrain. 

		typedef CEnemy Super;

		public:
			IBouncer();
			virtual ~IBouncer();

			virtual void OnFinishedCreating (const Daylon::FMetadata& Options) override;
			virtual void Tick               (float DeltaTime) override;
			virtual void Explode            (CGameObjectCollection&) override;
			
		protected:

			virtual void ResetFiringCountdown () = 0;
			virtual void FireWeapon           () = 0;

			float   FiringCountdown = 0.0f;

		private:
			float	Gravity;
	};


	class CBouncer : public IBouncer
	{
		typedef IBouncer Super;

		public:
			CBouncer();
			virtual ~CBouncer();

			virtual void FireWeapon           () override;
			virtual void ResetFiringCountdown () override;
	};


	class CWeakBouncer : public IBouncer
	{
		typedef IBouncer Super;

		public:
			CWeakBouncer();
			virtual ~CWeakBouncer();

			virtual void FireWeapon           () override;
			virtual void ResetFiringCountdown () override;
	};
}
