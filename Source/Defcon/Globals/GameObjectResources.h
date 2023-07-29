// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/*
	GameObjectResources.h
	Handles data for game object classes, such as sprite textures, sizes, etc.
	Loaded up by the PlayMmainWidget.
*/


#include "CoreMinimal.h"
#include "UDaylonSpriteWidget.h"
#include "GameObjects/obj_types.h"
#include "Runtime/Slate/Public/SlateMaterialBrush.h"


struct FGameObjectInfo
{
	// For each game object type, we need to know its Atlas, box size, and recommended radius (the last may be optional).

	TWeakObjectPtr<UDaylonSpriteWidgetAtlas> Atlas;
	FVector2D                                Size;
	float                                    Radius = 0.5f;
};


struct FGameObjectResources
{
	// todo: put brushes in their own manager object, or rename this type to FGameObjectResources
	FSlateBrush*   SmartbombBrushPtr      = nullptr;
	FSlateBrush*   DebrisBrushRoundPtr    = nullptr;
	FSlateBrush*   DebrisBrushSquarePtr   = nullptr;

	TMap<Defcon::ObjType, FGameObjectInfo> Data;

	void                    Add  (Defcon::ObjType Kind, const FGameObjectInfo& Info);
	const FGameObjectInfo&  Get  (Defcon::ObjType Kind) const;
};


extern FGameObjectResources GameObjectResources;