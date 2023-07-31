// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	GameColors.cpp
*/


#include "GameColors.h"

#include "Common/util_core.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"
#include "Common/gamma.h"
#include "Common/compat.h"



Defcon::CGameColors::CGameColors()
{
	CFVector ColorBases[(int32)EColor::numbases];
	
	ColorBases[(int32)EColor::gray        ].Set(1.0f, 1.0f, 1.0f);
	ColorBases[(int32)EColor::yellow      ].Set(1.0f, 1.0f, 0.0f);
	ColorBases[(int32)EColor::lightyellow ].Set(1.0f, 1.0f, 0.5f);
	ColorBases[(int32)EColor::red         ].Set(1.0f, 0.0f, 0.0f);
	ColorBases[(int32)EColor::blue        ].Set(0.0f, 0.0f, 1.0f);
	ColorBases[(int32)EColor::lightblue   ].Set(0.5f, 0.5f, 1.0f);
	ColorBases[(int32)EColor::purple      ].Set(0.7f, 0.0f, 0.7f);
	ColorBases[(int32)EColor::magenta     ].Set(1.0f, 0.0f, 1.0f);
	ColorBases[(int32)EColor::orange      ].Set(1.0f, 0.5f, 0.0f);
	ColorBases[(int32)EColor::green       ].Set(0.0f, 1.0f, 0.0f);

	for(int32 J = 0; J < array_size(ColorBases); J++)
	{
		for(int32 I = 0; I < array_size(Colors[0]); I++)
		{
			const float G = (float)I / (array_size(Colors[0]) - 1);
			//G = gamma(G);
			CFVector V(ColorBases[J]);
			V.Mul(G);

			Colors[J][I] = FLinearColor(V.x, V.y, V.z, 1.0f);
		}
	}
}


Defcon::CGameColors::~CGameColors()
{
}


FLinearColor Defcon::CGameColors::GetColor(Defcon::EColor Color, float Brightness) const
{
	Brightness = CLAMP(Brightness, 0.0f, 1.0f);
	const int32 n = ROUND(Brightness * (array_size(Colors[0]) - 1));

	return Colors[(int32)Color][n];
}


Defcon::CGameColors	gGameColors;

