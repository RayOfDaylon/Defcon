// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class IBouncer : public CEnemy
	{
		// A bouncer bounces on the terrain. 

		public:
			IBouncer();
			virtual ~IBouncer();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);

			void Explode(CGameObjectCollection&);
			

		private:
			float	Gravity;
	};


	class CBouncer : public IBouncer
	{
		// A bouncer bounces on the terrain. 

		public:
			CBouncer();
			virtual ~CBouncer();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			//virtual void Draw(FPaintArguments&, const I2DCoordMapper&);

			//void Explode(CGameObjectCollection&);
			
	};


	class CWeakBouncer : public IBouncer
	{
		// A bouncer bounces on the terrain. 

		public:
			CWeakBouncer();
			virtual ~CWeakBouncer();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			//virtual void Draw(FPaintArguments&, const I2DCoordMapper&);

			//void Explode(CGameObjectCollection&);
			
	};
}

