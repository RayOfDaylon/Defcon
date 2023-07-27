/*
	mine.cpp
	Mine weaponsfire routines for Defcon game.
	Copyright 2004 Daylon Graphics Ltd.
*/


#include "mine.h"







#include "Globals/prefs.h"
//#include "GameColors.h"

#include "Main/mapper.h"



// --------------------------------------------------------


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
	m_bboxrad.set(SpriteInfo.Size.X, SpriteInfo.Size.Y);
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
	//m_pos.muladd(m_orient.fwd, fTime * m_fSpeed );
}


void Defcon::CMine::DrawBbox(FPaintArguments&, const I2DCoordMapper&)
{
}


void Defcon::CMine::Draw
(
	FPaintArguments& framebuf, 
	const I2DCoordMapper& mapper
)
{
#if 0
	CFPoint pt;
	mapper.To(m_pos, pt);

	const float f = PSIN(m_fAge*PI*2);
	CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::mine0 + ROUND(f*2));

	int w = bmp.GetWidth();
	float brightness = (1.0f - m_fAge / MINE_LIFESPAN);
	brightness = FMath::Max(0.4f, brightness);
	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		pt.sub(CFPoint((float)w/2,
					(float)bmp.GetHeight()/2));
		bmp.BlitAlphaBrighten(
			framebuf, ROUND(pt.x), ROUND(pt.y), 
			w, bmp.GetHeight(), 
			0, 0, 
			brightness * PSIN(m_fAge*4)/2 + 0.5f);
	}
#endif
}


void Defcon::CMine::GetInjurePt(CFPoint& pt) const
{
	pt = m_pos;
}


bool Defcon::CMine::TestInjury(const CFRect& r) const
{
	return r.ptinside(m_pos);
}

