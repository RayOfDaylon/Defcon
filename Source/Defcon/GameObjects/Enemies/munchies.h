// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CPhred : public CEnemy
	{
		public:
			CPhred();
			virtual ~CPhred();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;


		private:
			bool	m_bPreferTargetUnderside;
			bool	m_bFacingLeft;
			float	m_fSquakTime;

	};

	class CBigRed : public CEnemy
	{
		public:
			CBigRed();
			virtual ~CBigRed();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;


		private:
			bool	m_bPreferTargetUnderside;
			bool	m_bFacingLeft;
			float	m_fSquakTime;

	};

	class CMunchie : public CEnemy
	{
		public:
			CMunchie();
			virtual ~CMunchie();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual int GetExplosionColorBase() const;
			float GetExplosionMass() const { return 0.6f; }


		private:
			bool	m_bPreferTargetUnderside;
			bool	m_bFacingLeft;
			float	m_fSquakTime;

	};
}
