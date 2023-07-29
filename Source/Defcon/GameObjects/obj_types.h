// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace Defcon
{
	enum class ObjType
	{
		PLAYER = 0,
		DESTROYED_PLAYER,
		HUMAN,		

		// Training objects
		BEACON,

		// Enemies.
		ENEMY,
		LANDER,
		HUNTER,
		GUPPY,
		BOMBER,
		BOMBER_LEFT, // not really an object type, just used to fetch left-facing texture atlas
		POD,
		SWARMER,
		
		BAITER,
		PHRED,
		BIGRED,
		MUNCHIE,

		FIREBOMBER,
		FIREBOMBER_TRUE,
		FIREBOMBER_WEAK,
		FIREBALL,
		DYNAMO,
		SPACEHUM,
		REFORMER,
		REFORMERPART,

		GHOST,
		GHOSTPART,
		
		BOUNCER,
		BOUNCER_TRUE,
		BOUNCER_WEAK,
		TURRET,

		EXPLOSION,
		MATERIALIZATION,
		FLAK,
		STAR,
		TEXT,
		TEXTURE,
		STARGATE,

		// Used in other arenas.
		STATICTEXT,
		DRAWING,
		MISSIONTILE,
		SLIDER,
		//STORY,		

		// Weapons fire.
		MINE,
		SMARTBOMB,
		LASERBEAM,
		BULLET,
		BULLET_ROUND,
		BULLET_THIN,

		UNKNOWN//,
		//COUNT
	};

	

	struct FObjectTypeManager
	{
		public:

		void Init();
		const FString& GetName(ObjType Kind) { return TypeNames[Kind]; }

		protected:

		TMap<ObjType, FString> TypeNames;
	};

	extern FObjectTypeManager ObjectTypeManager;
}