// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once



#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"



namespace Defcon
{
	class CStargate : public IGameObject
	{
		// A hyperspace portal used to conclude missions
		// and to teleport to the highest priority abduction.

		public:
			CStargate();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void DrawSmall (FPainter&, const I2DCoordMapper&, FSlateBrush&) override;
			

		private:

			void DrawSmallPart(FPainter& PaintArgs, const I2DCoordMapper& mapper, FSlateBrush& Brush, const CFPoint& TopLeft, const CFPoint& BottomRight, const CFPoint&);
	};
}


