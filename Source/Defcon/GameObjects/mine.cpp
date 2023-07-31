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
	bCanBeInjured = false;

	RadarColor = C_DARK;

	m_color = MakeColorFromComponents(255, 255, 255);
	bMortal = true;
	Lifespan = SFRAND * 0.25f + MINE_LIFESPAN;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
}


#ifdef _DEBUG
const char* Defcon::CMine::GetClassname() const
{
	static char* psz = "Mine";
	return psz;
}
#endif



void Defcon::CMine::Move(float fTime)
{
	Age += fTime;
}


void Defcon::CMine::DrawBbox(FPaintArguments&, const I2DCoordMapper&)
{
}


void Defcon::CMine::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CMine::GetInjurePt(CFPoint& pt) const
{
	pt = Position;
}


bool Defcon::CMine::TestInjury(const CFRect& r) const
{
	return r.PtInside(Position);
}
