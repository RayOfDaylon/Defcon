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

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float) override;
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&) override;

			void Explode(CGameObjectCollection&) override;
			

		protected:
			CFPoint m_ourInertia;
			float   m_firingCountdown = 0.0f;
			float   m_travelCountdown = 0.0f;
			int		m_sgn;
			bool	m_bAbsSin;
			bool	m_bWaits;
	};


	class CFirebomber : public IFirebomber
	{
		// A firebomber that shoots fireballs. 

		typedef IFirebomber Super;

		public:
			CFirebomber();
			virtual ~CFirebomber();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float) override;
	};


	class CWeakFirebomber : public IFirebomber
	{
		// A firebomber that shoots bullets. 

		typedef IFirebomber Super;

		public:
			CWeakFirebomber();
			virtual ~CWeakFirebomber();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float) override;
	};
}

