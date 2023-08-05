// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// terrain.h

#include "CoreMinimal.h"

#include "gameobj.h"


struct FPainter;


namespace Defcon
{
	class I2DCoordMapper;

	class CTerrain : public IGameObject
	{
		// The mountain range appears on the bottom of the arena and 
		// provides a strong frame of reference for moving about. 
		// todo: add volcano that continuously emits particles.

		public:
			CTerrain();
			virtual ~CTerrain();

			void          InitTerrain (float W, float H);
			virtual void  Draw        (FPainter&, const I2DCoordMapper&) override;
			virtual void  DrawSmall   (FPainter&, const I2DCoordMapper&, FSlateBrush&) override;
			float	      GetElev     (float X) const;

		private:
			TArray<FVector2D>   Vertices;
			TArray<float>       Elevations;
			float               ArenaWidth;
			float               ArenaHeight;
	};
}
