// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CBaiter : public CEnemy
	{
		// A bomber is a square purple guy that flies 
		// in a sine wave and lays immoble indestructibe mines.
		// One of the most hated enemies.
		public:
			CBaiter();
			virtual ~CBaiter();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;


		private:
			bool m_bPreferTargetUnderside;

	};
}
