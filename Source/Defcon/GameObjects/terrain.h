// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// terrain.h


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"

#include "gameobj.h"
#include "gameobjlive.h"
#include "obj_types.h"



namespace Defcon
{
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

			void         InitTerrain (int, int, int, int);
			virtual void Move        (float);
			virtual void Draw        (FPaintArguments&, const I2DCoordMapper&) override;
			virtual void DrawSmall   (FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override;
			float	     GetElev     (float) const;

		private:
			TArray<FVector2D>   Vertices;
			int					m_width;
			int					m_maxheight;
			std::vector<int>	m_elevs;
			bool*				m_deltas;
			bool				m_firsttime;

	}; // CTerrain
}
