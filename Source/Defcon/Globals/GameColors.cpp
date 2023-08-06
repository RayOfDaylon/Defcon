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
	FVector3f ColorBases[(int32)EColor::Count];
	
	ColorBases[(int32)EColor::Gray        ].Set(1.0f, 1.0f, 1.0f);
	ColorBases[(int32)EColor::Yellow      ].Set(1.0f, 1.0f, 0.0f);
	ColorBases[(int32)EColor::LightYellow ].Set(1.0f, 1.0f, 0.5f);
	ColorBases[(int32)EColor::Red         ].Set(1.0f, 0.0f, 0.0f);
	ColorBases[(int32)EColor::Blue        ].Set(0.0f, 0.0f, 1.0f);
	ColorBases[(int32)EColor::LightBlue   ].Set(0.5f, 0.5f, 1.0f);
	ColorBases[(int32)EColor::Purple      ].Set(0.7f, 0.0f, 0.7f);
	ColorBases[(int32)EColor::Magenta     ].Set(1.0f, 0.0f, 1.0f);
	ColorBases[(int32)EColor::Orange      ].Set(1.0f, 0.5f, 0.0f);
	ColorBases[(int32)EColor::Green       ].Set(0.0f, 1.0f, 0.0f);

	for(int32 J = 0; J < array_size(ColorBases); J++)
	{
		for(int32 I = 0; I < array_size(Colors[0]); I++)
		{
			const float G = (float)I / (array_size(Colors[0]) - 1);
			//G = gamma(G);
			FVector V(ColorBases[J]);
			V *= G;

			Colors[J][I] = FLinearColor(V);
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

