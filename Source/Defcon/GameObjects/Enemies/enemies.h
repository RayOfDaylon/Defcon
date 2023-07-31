// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// enemies.h

#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"

#include "GameObjects/gameobjlive.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"



namespace Defcon
{
	class CEnemy : public ILiveGameObject
	{
		// Base class for all enemy objects.

		typedef ILiveGameObject Super;


		public:

			CEnemy();
			virtual ~CEnemy();

			virtual void   Move          (float) override;

			virtual void   SetTarget     (IGameObject* p)    { TargetPtr = p; }
			virtual bool   Fireballs     () const override   { return true; }


		protected:

			IGameObject*    TargetPtr = nullptr;

			float           Brightness;
			float           ControlStartTime[Super::numCtls];
	};
}

