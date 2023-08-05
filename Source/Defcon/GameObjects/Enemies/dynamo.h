// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CDynamo : public CEnemy
	{
		// A dynamo spins and floats around and
		// emits space hums.

		public:
			CDynamo();
			virtual ~CDynamo();

			virtual void Move (float DeltaTime) override;

		private:
			float	 OriginalY;
			float    SpawnSpacehumCountdown = 1.0f;
			CFPoint  WiggleAnimSpeed;
	};
}

