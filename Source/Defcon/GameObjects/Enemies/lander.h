// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CLander : public CEnemy
	{
		public:
			CLander();
			virtual ~CLander();

			virtual void Notify(Defcon::Message, void*);

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;

			void SetDoChaseHumans(bool b) { m_bChaseNearestHuman = b; }

			CGameObjectCollection*	m_pObjects;
			float					m_personalSpace;
			float					m_maxSpeed;

			// Hover mode stuff.
			float					m_fHoverAltitude; // Distance above ground, not absolute Y value.
			void*					m_pvUserTerrainEval;

			virtual void OnAboutToDie();

		protected:

		private:
			enum class State { descending, hovering, acquiring, ascending, ascended, fighting };
			State					m_eState;
			float					m_fSpeed;
			IGameObject*			m_pHuman;
			IGameObject*			m_pTrackedHuman;
			bool					m_bChaseNearestHuman;
			bool					m_bAscendStraight;
	};
}
