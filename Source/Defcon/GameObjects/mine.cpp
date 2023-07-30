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
	m_parentType = m_type;
	m_type = ObjType::MINE;
	m_bInjurious = true;
	m_bCanBeInjured = false;

	m_smallColor = C_DARK;

	m_color = MakeColorFromComponents(255, 255, 255);
	m_bMortal = true;
	m_fLifespan = SFRAND * 0.25f + MINE_LIFESPAN;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.Set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
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
	m_fAge += fTime;
}


void Defcon::CMine::DrawBbox(FPaintArguments&, const I2DCoordMapper&)
{
}


void Defcon::CMine::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CMine::GetInjurePt(CFPoint& pt) const
{
	pt = m_pos;
}


bool Defcon::CMine::TestInjury(const CFRect& r) const
{
	return r.PtInside(m_pos);
}
