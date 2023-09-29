// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "obj_types.h"


Defcon::FObjectTypeManager Defcon::GObjectTypeManager;


void Defcon::FObjectTypeManager::Init()
{
	TypeNames.Reset();

#define ADD_TYPENAME(_name)  TypeNames.Add(EObjType::_name, #_name);

	ADD_TYPENAME(BAITER);


	ADD_TYPENAME(BAITER               )
	ADD_TYPENAME(BEACON               )
	ADD_TYPENAME(BIGRED               )
	ADD_TYPENAME(BOMBER               )
	ADD_TYPENAME(BOUNCER              )
	ADD_TYPENAME(BOUNCER_TRUE         )
	ADD_TYPENAME(BOUNCER_WEAK         )
	ADD_TYPENAME(BULLET               )
	ADD_TYPENAME(BULLET_ROUND         )
	ADD_TYPENAME(BULLET_THIN          )
	ADD_TYPENAME(DESTROYED_PLAYER     )
	ADD_TYPENAME(DYNAMO               )
	ADD_TYPENAME(EXPLOSION            )
	ADD_TYPENAME(EXPLOSION2           )
	ADD_TYPENAME(FIREBALL             )
	ADD_TYPENAME(FIREBOMBER           )
	ADD_TYPENAME(FIREBOMBER_TRUE      )
	ADD_TYPENAME(FIREBOMBER_WEAK      )
	ADD_TYPENAME(FLAK                 )
	ADD_TYPENAME(GHOST                )
	ADD_TYPENAME(GHOSTPART            )
	ADD_TYPENAME(GUPPY                )
	ADD_TYPENAME(HUMAN                )
	ADD_TYPENAME(HUNTER               )
	ADD_TYPENAME(LANDER               )
	ADD_TYPENAME(LASERBEAM            )
	ADD_TYPENAME(MATERIALIZATION      )
	ADD_TYPENAME(MINE                 )
	ADD_TYPENAME(IMUNCHIE             )
	ADD_TYPENAME(MUNCHIE              )
	ADD_TYPENAME(PHRED                )
	ADD_TYPENAME(PLAYER               )
	ADD_TYPENAME(POD                  )
	ADD_TYPENAME(POWERUP              )
	ADD_TYPENAME(POWERUP_SHIELDS      )
	ADD_TYPENAME(POWERUP_DUALCANNON   )
	ADD_TYPENAME(POWERUP_INVINCIBILITY)
	ADD_TYPENAME(REFORMER             )
	ADD_TYPENAME(REFORMERPART         )
	ADD_TYPENAME(SMARTBOMB            )
	ADD_TYPENAME(SPACEHUM             )
	ADD_TYPENAME(STAR                 )
	ADD_TYPENAME(STARGATE             )
	ADD_TYPENAME(SWARMER              )
	ADD_TYPENAME(TEXTURE              )
	ADD_TYPENAME(TURRET               )
	ADD_TYPENAME(UNKNOWN              )
}
	
#undef ADD_TYPENAME



