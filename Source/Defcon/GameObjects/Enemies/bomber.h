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

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			//virtual	void Init  (const CFPoint& Size1, const CFPoint& Size2) override;
			virtual void OnFinishedCreating () override;

			virtual void Move  (float);
			virtual void Draw  (FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;
			virtual float GetExplosionMass() const;

		private:
			float m_fLayingMines;
			float m_yoff;
			float m_freq;
			float m_xFreq;
			float m_amp;
			float m_halfwayAltitude;

			CBezierSpline2D m_currentPath;
			float m_pathTravelTime = 0.0f;
			float m_secondsPerPath = 0.0f;
			CFPoint m_posPrev;
	};
}

