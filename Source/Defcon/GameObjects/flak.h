// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// flak.h

#include "Common/Painter.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"
#include "GameObjects/gameobj.h"
#include "GameObjects/gameobjlive.h"

#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"


namespace Defcon
{
	class CFlak : public IGameObject
	{
		// A flak object is just a piece of debris used in explosions. 
		// It has a random appearance per instance.

		public:
			CFlak();
			virtual ~CFlak();

			virtual void Tick      (float DeltaTime) override;
			virtual void Draw      (FPainter&, const I2DCoordMapper&) override;
			virtual void DrawSmall (FPainter&, const I2DCoordMapper&, FSlateBrush&) override {}

			virtual bool OccursFrequently () const override { return true; }

			float	LargestSize = 6;
			EColor	ColorbaseOld;
			EColor	ColorbaseYoung;
			bool	bFade = true;
			bool	bCold;

		protected:
			float          MaxAge;
			FSlateBrush*   BrushPtr = nullptr;
	};


	class CGlowingFlak : public CFlak
	{
		public:
			//CGlowingFlak();
			//virtual ~CGlowingFlak();

			virtual void Draw(FPainter&, const I2DCoordMapper&) override;
	};
}



