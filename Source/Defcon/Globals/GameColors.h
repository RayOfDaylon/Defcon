// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/util_core.h"
#include "Common/util_math.h"
#include "Common/util_color.h"


namespace Defcon
{
	class CGameColors
	{
		public:
			CGameColors();
			virtual ~CGameColors();

			enum 
			{ 
				gray,
				yellow,
				lightyellow,
				red,
				blue,
				lightblue,
				purple,
				magenta,
				orange,
				green,
				numbases
			};

			FLinearColor GetColor(int c, float f) const
				{
					f = FMath::Max(0, f);
					f = FMath::Min(1.0f, f);
					int n = ROUND(f * (array_size(m_c[0])-1));
					return m_c[c][n];
				}

		private:
			FLinearColor	m_c[numbases][128];
	}; // CGameColors

}

extern Defcon::CGameColors	gGameColors;

