// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "obj_types.h"


Defcon::FObjectTypeManager Defcon::ObjectTypeManager;


void Defcon::FObjectTypeManager::Init()
{
	TypeNames.Reset();

	TypeNames.Add(ObjType::BAITER          , "BAITER");
	TypeNames.Add(ObjType::BEACON          , "BEACON");
	TypeNames.Add(ObjType::BIGRED          , "BIGRED");
	TypeNames.Add(ObjType::BOMBER          , "BOMBER");
	TypeNames.Add(ObjType::BOUNCER         , "BOUNCER");
	TypeNames.Add(ObjType::BOUNCER_TRUE    , "BOUNCER_TRUE");
	TypeNames.Add(ObjType::BOUNCER_WEAK    , "BOUNCER_WEAK");
	TypeNames.Add(ObjType::BULLET          , "BULLET");
	TypeNames.Add(ObjType::BULLET_ROUND    , "BULLET_ROUND");
	TypeNames.Add(ObjType::BULLET_THIN     , "BULLET_THIN");
	TypeNames.Add(ObjType::DESTROYED_PLAYER, "DESTROYED_PLAYER");
	TypeNames.Add(ObjType::DRAWING         , "DRAWING");
	TypeNames.Add(ObjType::DYNAMO          , "DYNAMO");
	TypeNames.Add(ObjType::EXPLOSION       , "EXPLOSION");
	TypeNames.Add(ObjType::FIREBALL        , "FIREBALL");
	TypeNames.Add(ObjType::FIREBOMBER      , "FIREBOMBER");
	TypeNames.Add(ObjType::FIREBOMBER_TRUE , "FIREBOMBER_TRUE");
	TypeNames.Add(ObjType::FIREBOMBER_WEAK , "FIREBOMBER_WEAK");
	TypeNames.Add(ObjType::FLAK            , "FLAK");
	TypeNames.Add(ObjType::GHOST           , "GHOST");
	TypeNames.Add(ObjType::GHOSTPART       , "GHOSTPART");
	TypeNames.Add(ObjType::GUPPY           , "GUPPY");
	TypeNames.Add(ObjType::HUMAN           , "HUMAN");
	TypeNames.Add(ObjType::HUNTER          , "HUNTER");
	TypeNames.Add(ObjType::LANDER          , "LANDER");
	TypeNames.Add(ObjType::LASERBEAM       , "LASERBEAM");
	TypeNames.Add(ObjType::MATERIALIZATION , "MATERIALIZATION");
	TypeNames.Add(ObjType::MINE            , "MINE");
	TypeNames.Add(ObjType::MISSIONTILE     , "MISSIONTILE");
	TypeNames.Add(ObjType::MUNCHIE         , "MUNCHIE");
	TypeNames.Add(ObjType::PHRED           , "PHRED");
	TypeNames.Add(ObjType::PLAYER          , "PLAYER");
	TypeNames.Add(ObjType::POD             , "POD");
	TypeNames.Add(ObjType::REFORMER        , "REFORMER");
	TypeNames.Add(ObjType::REFORMERPART    , "REFORMERPART");
	TypeNames.Add(ObjType::SLIDER          , "SLIDER");
	TypeNames.Add(ObjType::SMARTBOMB       , "SMARTBOMB");
	TypeNames.Add(ObjType::SPACEHUM        , "SPACEHUM");
	TypeNames.Add(ObjType::STAR            , "STAR");
	TypeNames.Add(ObjType::STARGATE        , "STARGATE");
	TypeNames.Add(ObjType::STATICTEXT      , "STATICTEXT");
	TypeNames.Add(ObjType::SWARMER         , "SWARMER");
	TypeNames.Add(ObjType::TEXT            , "TEXT");
	TypeNames.Add(ObjType::TEXTURE         , "TEXTURE");
	TypeNames.Add(ObjType::TURRET          , "TURRET");
	TypeNames.Add(ObjType::UNKNOWN         , "UNKNOWN");
}
	
	



