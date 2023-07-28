// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "GameObjects/GameObjectCollection.h"

#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"


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

			CFPoint					m_range;
			I2DCoordMapper*			m_pMapper  = nullptr;
			CGameObjectCollection*	m_pTargets = nullptr;
			CGameObjectCollection*	m_pDebris  = nullptr;
			UDefconPlayViewBase*	m_pArena   = nullptr; // todo: use gpArena

		protected:
			UMaterialInstanceDynamic* MID;
			FName                     NameColor;
			FName                     NameOs;
	};
}


