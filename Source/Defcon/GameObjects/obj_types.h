// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace Defcon
{
	enum class EObjType
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
		IMUNCHIE,
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
		EXPLOSION2,
		HEART,
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

		// Powerups
		POWERUP,
		POWERUP_SHIELDS,
		POWERUP_DOUBLEGUNS,
		POWERUP_INVINCIBILITY,

		UNKNOWN//,
		//COUNT
	};

	

	struct FObjectTypeManager
	{
		public:

		void Init();
		const FString& GetName(EObjType Kind) { return TypeNames[Kind]; }

		protected:

		TMap<EObjType, FString> TypeNames;
	};

	extern FObjectTypeManager GObjectTypeManager;
}
