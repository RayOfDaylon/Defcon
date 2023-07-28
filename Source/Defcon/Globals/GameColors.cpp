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
	CFVector cbase[numbases];
	
	cbase[gray].		set(1.0f, 1.0f, 1.0f);
	cbase[yellow].		set(1.0f, 1.0f, 0.0f);
	cbase[lightyellow].	set(1.0f, 1.0f, 0.5f);
	cbase[red].			set(1.0f, 0.0f, 0.0f);
	cbase[blue].		set(0.0f, 0.0f, 1.0f);
	cbase[lightblue].	set(0.5f, 0.5f, 1.0f);
	cbase[purple].		set(0.7f, 0.0f, 0.7f);
	cbase[magenta].		set(1.0f, 0.0f, 1.0f);
	cbase[orange].		set(1.0f, 0.5f, 0.0f);
	cbase[green].		set(0.0f, 1.0f, 0.0f);


	for(int j = 0; j < numbases; j++)
	{
		for(int i = 0; i < array_size(m_c[0]); i++)
		{
			float g = (float)i / (array_size(m_c[0]) - 1);
			//g = gamma(g);
			CFVector v(cbase[j]);
			v.mul(g * 255);
			FLinearColor c = MakeColorFromComponents(v.x, v.y, v.z);
			m_c[j][i] = c;
		}
	}
}


Defcon::CGameColors::~CGameColors()
{
}


Defcon::CGameColors	gGameColors;

