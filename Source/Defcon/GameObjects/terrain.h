// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// terrain.h

#include "CoreMinimal.h"

#include "gameobj.h"


struct FPaintArguments;


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

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			void          InitTerrain (int32 seed, int32 w, int32 h, int32 diff);
			virtual void  Move        (float);
			virtual void  Draw        (FPaintArguments&, const I2DCoordMapper&) override;
			virtual void  DrawSmall   (FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override;
			float	      GetElev     (float) const;

		private:
			TArray<FVector2D>   Vertices;
			TArray<int32>       m_elevs;
			bool*				m_deltas;
			int                 m_width;
			int                 m_maxheight;
			bool				m_firsttime;

	}; // CTerrain
}
