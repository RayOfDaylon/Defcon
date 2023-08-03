// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/Painter.h"
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
			virtual void Draw(FPainter&, const I2DCoordMapper&);

			CFPoint                   Range;
			I2DCoordMapper*           MapperPtr  = nullptr;
			CGameObjectCollection*    Targets    = nullptr;
			CGameObjectCollection*    Debris     = nullptr;

		protected:
			UMaterialInstanceDynamic* MID;
			FName                     NameColor;
			FName                     NameOs;
	};
}
