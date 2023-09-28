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
	class CSmartbombShockwave : public IGameObject
	{
		typedef IGameObject Super;

		public:
			CSmartbombShockwave();

			virtual void Tick (float DeltaTime) override;
			virtual void Draw (FPainter&, const I2DCoordMapper&) override;

			void         InitSmartbombShockwave (const CFPoint& InRange, const I2DCoordMapper* InMapper, CGameObjectCollection* InTargets, CGameObjectCollection* InDebris);


		protected:

			UMaterialInstanceDynamic* MID;
			FName                     NameColor;
			FName                     NameOs;

			CFPoint                   Range;
			CGameObjectCollection*    Targets  = nullptr;
			CGameObjectCollection*    Debris   = nullptr;

			bool                      DestroyingPodIntersection = false;
	};
}
