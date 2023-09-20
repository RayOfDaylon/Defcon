// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CPod : public CEnemy
	{
		// A pod floats around and releases a swarmer squadron when it explodes. 
		// Because of the way the collision physics works, you can destroy a pod
		// and all of its swarmers if you collide perfectly with it. A glancing
		// collision will have some swarmers survive.

		typedef CEnemy Super;

		public:
			CPod();
			virtual ~CPod();

			virtual void OnFinishedCreating  (const Daylon::FMetadata& Options) override;
			virtual void Tick                (float DeltaTime) override;
			virtual void OnAboutToDie        () override;
			virtual void Explode             (CGameObjectCollection&) override;

			void SetSpeed(float InSpeed) { Speed = InSpeed; }


		private:

			float OffsetY;   // todo: this doesn't do anything?
			float Frequency;
			float Speed = 0.0f;
	};
}
