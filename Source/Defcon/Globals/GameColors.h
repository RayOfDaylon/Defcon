// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/util_core.h"
#include "Common/util_math.h"
#include "Common/util_color.h"


namespace Defcon
{
	enum class EColor
	{
		Gray,
		Yellow,
		LightYellow,
		Red,
		Blue,
		LightBlue,
		Purple,
		Magenta,
		Orange,
		Green,
		Count
	};

	// gray, yellow, and lightyellow need to be the top three.

	class CGameColors
	{
		public:
			CGameColors();
			virtual ~CGameColors();

			FLinearColor GetColor(EColor Color, float Brightness) const;

		private:
			FLinearColor	Colors[(int32)EColor::Count][32];
	};
}

extern Defcon::CGameColors	GGameColors;

