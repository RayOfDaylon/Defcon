// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


/*
	GameObjectResources.cpp
	Handles data for game object classes, such as sprite textures, sizes, etc.
	Loaded up by the PlayMmainWidget.
*/


#include "GameObjectResources.h"


FGameObjectResources GameObjectResources;



void FGameObjectResources::Add(Defcon::EObjType Kind, const FGameObjectInfo& Info)
{
	if(Data.Find(Kind))
	{
		return;
	}

	Data.Add(Kind, Info);
}


const FGameObjectInfo& FGameObjectResources::Get(Defcon::EObjType Kind) const
{
	static FGameObjectInfo NullGameObjectInfo;

	auto Ptr = Data.Find(Kind);

	if(Ptr == nullptr)
	{
		return NullGameObjectInfo;
	}

	return *Ptr;
}
