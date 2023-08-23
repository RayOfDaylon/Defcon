// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CBomber : public CEnemy
	{
		// A bomber is a square purple guy that flies 
		// in a sine wave and lays immoble indestructibe mines.
		// One of the most hated enemies.

		typedef CEnemy Super;

		public:
			CBomber();

			virtual void    OnFinishedCreating    () override;

			virtual void    Tick                  (float DeltaTime) override;
			virtual EColor  GetExplosionColorBase () const override;
			virtual float   GetExplosionMass      () const override;

		private:
			float           StopLayingMinesCountdown;
			CBezierSpline2D CurrentPath;
			float           PathTravelTime = 0.0f;
			float           SecondsPerPath = 0.0f;
			CFPoint         PreviousPosition;
	};
}

