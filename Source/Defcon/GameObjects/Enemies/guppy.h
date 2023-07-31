// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "enemies.h"


namespace Defcon
{
	class CGuppy : public CEnemy
	{
		public:
			CGuppy();

			//virtual void Notify(Defcon::EMessage, void*);

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;
			virtual float GetExplosionMass() const;
			bool Fireballs() const { return false; }
			
			void Explode(CGameObjectCollection&);

			CGameObjectCollection*	m_pObjects;
			float					m_personalSpace;
			float					m_maxSpeed;

			virtual void OnAboutToDie();

			float					m_fTimeTargetWithinRange;
			CFPoint					m_targetOffset;

		protected:
			void  ConsiderFiringBullet();

		private:
			enum State { lounging, fighting, evading };
			State					m_eState;
			float					m_fSpeed;
			float					m_freq;
			float					m_amp;
			float                   m_firingCountdown = 0.0f;
			
	};
}
