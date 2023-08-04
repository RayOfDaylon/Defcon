// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "obj_types.h"


Defcon::FObjectTypeManager Defcon::GObjectTypeManager;


void Defcon::FObjectTypeManager::Init()
{
	TypeNames.Reset();

	TypeNames.Add(EObjType::BAITER          , "BAITER");
	TypeNames.Add(EObjType::BEACON          , "BEACON");
	TypeNames.Add(EObjType::BIGRED          , "BIGRED");
	TypeNames.Add(EObjType::BOMBER          , "BOMBER");
	TypeNames.Add(EObjType::BOUNCER         , "BOUNCER");
	TypeNames.Add(EObjType::BOUNCER_TRUE    , "BOUNCER_TRUE");
	TypeNames.Add(EObjType::BOUNCER_WEAK    , "BOUNCER_WEAK");
	TypeNames.Add(EObjType::BULLET          , "BULLET");
	TypeNames.Add(EObjType::BULLET_ROUND    , "BULLET_ROUND");
	TypeNames.Add(EObjType::BULLET_THIN     , "BULLET_THIN");
	TypeNames.Add(EObjType::DESTROYED_PLAYER, "DESTROYED_PLAYER");
	TypeNames.Add(EObjType::DRAWING         , "DRAWING");
	TypeNames.Add(EObjType::DYNAMO          , "DYNAMO");
	TypeNames.Add(EObjType::EXPLOSION       , "EXPLOSION");
	TypeNames.Add(EObjType::FIREBALL        , "FIREBALL");
	TypeNames.Add(EObjType::FIREBOMBER      , "FIREBOMBER");
	TypeNames.Add(EObjType::FIREBOMBER_TRUE , "FIREBOMBER_TRUE");
	TypeNames.Add(EObjType::FIREBOMBER_WEAK , "FIREBOMBER_WEAK");
	TypeNames.Add(EObjType::FLAK            , "FLAK");
	TypeNames.Add(EObjType::GHOST           , "GHOST");
	TypeNames.Add(EObjType::GHOSTPART       , "GHOSTPART");
	TypeNames.Add(EObjType::GUPPY           , "GUPPY");
	TypeNames.Add(EObjType::HUMAN           , "HUMAN");
	TypeNames.Add(EObjType::HUNTER          , "HUNTER");
	TypeNames.Add(EObjType::LANDER          , "LANDER");
	TypeNames.Add(EObjType::LASERBEAM       , "LASERBEAM");
	TypeNames.Add(EObjType::MATERIALIZATION , "MATERIALIZATION");
	TypeNames.Add(EObjType::MINE            , "MINE");
	TypeNames.Add(EObjType::MISSIONTILE     , "MISSIONTILE");
	TypeNames.Add(EObjType::IMUNCHIE        , "IMUNCHIE");
	TypeNames.Add(EObjType::MUNCHIE         , "MUNCHIE");
	TypeNames.Add(EObjType::PHRED           , "PHRED");
	TypeNames.Add(EObjType::PLAYER          , "PLAYER");
	TypeNames.Add(EObjType::POD             , "POD");
	TypeNames.Add(EObjType::REFORMER        , "REFORMER");
	TypeNames.Add(EObjType::REFORMERPART    , "REFORMERPART");
	TypeNames.Add(EObjType::SLIDER          , "SLIDER");
	TypeNames.Add(EObjType::SMARTBOMB       , "SMARTBOMB");
	TypeNames.Add(EObjType::SPACEHUM        , "SPACEHUM");
	TypeNames.Add(EObjType::STAR            , "STAR");
	TypeNames.Add(EObjType::STARGATE        , "STARGATE");
	TypeNames.Add(EObjType::STATICTEXT      , "STATICTEXT");
	TypeNames.Add(EObjType::SWARMER         , "SWARMER");
	TypeNames.Add(EObjType::TEXT            , "TEXT");
	TypeNames.Add(EObjType::TEXTURE         , "TEXTURE");
	TypeNames.Add(EObjType::TURRET          , "TURRET");
	TypeNames.Add(EObjType::UNKNOWN         , "UNKNOWN");
}
	
	



