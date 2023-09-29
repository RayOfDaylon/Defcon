// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace Defcon
{
	enum class EObjType
	{
		// todo: we've mixed texture IDs in this enum when they should be in their own list, 
		// no matter how closely it matches this list.

		PLAYER = 0,
		DESTROYED_PLAYER,
		INVINCIBLE_PLAYER,
		HUMAN,		

		// Training objects
		BEACON,

		// Enemies.
		ENEMY,

		BAITER,
		BIGRED,
		BOMBER,
		BOMBER_LEFT, // Not an object type; used to fetch left-facing texture atlas.
		BOUNCER,
		BOUNCER_TRUE,
		BOUNCER_WEAK,
		DYNAMO,
		FIREBALL,
		FIREBOMBER,
		FIREBOMBER_TRUE,
		FIREBOMBER_WEAK,
		GHOST,
		GHOSTPART,
		GUPPY,
		HUNTER,
		IMUNCHIE,
		LANDER,
		MUNCHIE,
		PHRED,
		POD,
		REFORMER,
		REFORMERPART,
		SPACEHUM,
		SWARMER,
		TURRET,

		// Other stuff.
		EXPLOSION,
		EXPLOSION2,
		HEART,
		MATERIALIZATION,
		FLAK,             // We don't use this, but we might later.
		STAR,
		TEXTURE,
		STARGATE,

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
		POWERUP_DUALCANNON,
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
