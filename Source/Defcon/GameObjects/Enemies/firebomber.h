// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class IFirebomber : public CEnemy
	{
		// A firebomber floats around but moves up and down a lot 
		// and spins, releasing weapons fire now and then at the player. 

		typedef CEnemy Super;

		public:
			IFirebomber();
			virtual ~IFirebomber();

			virtual void Tick(float DeltaTime) override;

			void Explode(CGameObjectCollection&) override;
			

		protected:
			CFPoint OurInertia;
			float   FiringCountdown = 0.0f;
			float   TravelCountdown = 0.0f;
	};


	class CFirebomber : public IFirebomber
	{
		// A firebomber that shoots fireballs. 

		typedef IFirebomber Super;

		public:
			CFirebomber();
			virtual ~CFirebomber();

			virtual void Tick(float DeltaTime) override;
	};


	class CWeakFirebomber : public IFirebomber
	{
		// A firebomber that shoots bullets. 

		typedef IFirebomber Super;

		public:
			CWeakFirebomber();
			virtual ~CWeakFirebomber();

			virtual void Tick(float DeltaTime) override;
	};
}

