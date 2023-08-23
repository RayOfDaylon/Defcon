// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mine.cpp
	Mine weaponsfire routines for Defcon game.
*/


#include "mine.h"

#include "Globals/prefs.h"
#include "Main/mapper.h"


Defcon::CMine::CMine()
{
	ParentType = Type;
	Type = EObjType::MINE;
	bInjurious = true;
	bCanBeInjured = false; // todo: what if we let mines have a tiny bbox requiring precise shots to destroy them?

	//RadarColor = C_DARK;

	//m_color = C_WHITE;
	bMortal = true;
	Lifespan = FRANDRANGE(MINE_LIFESPAN - 0.25f, MINE_LIFESPAN + 0.25f);//  SFRAND * 0.25f + MINE_LIFESPAN;

	CreateSprite(Type);
	const auto& SpriteInfo = GGameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
}


void Defcon::CMine::Tick(float DeltaTime)
{
	Age += DeltaTime;

	// todo: might be cool to have a mine explode if it dies
}


void Defcon::CMine::GetInjurePt(CFPoint& Pt) const
{
	Pt = Position;
}


bool Defcon::CMine::TestInjury(const CFRect& R) const
{
	return R.PtInside(Position);
}
