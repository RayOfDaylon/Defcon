// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once




#include "Common/PaintArguments.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobj.h"


class UDefconPlayViewBase;

namespace Defcon
{
	class CSmartbomb : public IGameObject
	{
		public:
			CSmartbomb();
#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);

			I2DCoordMapper*			m_pMapper = nullptr;
			CFPoint					m_range;
			CGameObjectCollection*	m_pTargets = nullptr;
			CGameObjectCollection*	m_pDebris = nullptr;
			UDefconPlayViewBase*	m_pArena = nullptr;

		protected:
			UMaterialInstanceDynamic* MID;
			FName                     NameColor;
			FName                     NameOs;
	};
}


