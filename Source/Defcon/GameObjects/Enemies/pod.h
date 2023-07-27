// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CPod : public CEnemy
	{
		// A pod floats around and releases a swarmer
		// squadron when it explodes. 

		public:
			CPod();
			virtual ~CPod();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual void OnAboutToDie();
			void Explode(CGameObjectCollection&);
			

			float m_yoff;
			float m_freq;
			float m_xFreq;
			float m_yOrg;
	};
}

