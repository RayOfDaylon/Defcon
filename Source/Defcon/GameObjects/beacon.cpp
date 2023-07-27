#include "beacon.h"

#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "Main/mapper.h"
#include "DefconLogging.h"


Defcon::CBeacon::CBeacon()
{
	m_parentType = m_type;
	m_type = ObjType::BEACON;
	m_bMortal = false;

	// Spawn our sprite here.

	CreateSprite(ObjType::BEACON);
}


Defcon::CBeacon::~CBeacon()
{
}


void Defcon::CBeacon::Move(float DeltaTime)
{
	// Beacons don't move, but they do flash, and we want to show that in the radar color.

	// Oscillate between yellow and brown.

	m_fAge += DeltaTime;

	float T = (sin(m_fAge * PI * 2) + 1) * 0.5f;

	LerpColor(C_YELLOW, C_BROWN, T, m_smallColor);
}



void Defcon::CBeacon::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
#if 0
	if(this->IsMaterializing())
		return;

	if(this->CollisionCount() > 0)
		return;

	const int nFrame = ROUND(PSIN(m_fAge*PI*4) * (CBitmaps::beacon_count - 1));
	CTrueBitmap& bmp = gBitmaps.GetBitmap(CBitmaps::beacon0 + nFrame);

	CFPoint pt;
	mapper.To(m_pos, pt);

	pt -= m_bboxrad;

	bmp.BlitAlpha(framebuf, (int)pt.x, (int)pt.y, bmp.GetWidth(), bmp.GetHeight(), 0, 0);

	m_smallColor = gGameColors.GetColor(CGameColors::yellow, MAP(nFrame, 0, 3, 0.5f, 1.0f));

#if 0
	if(this->CollisionCount() > 0)
		return;//m_nFrame = 3;
	else
		m_nFrame = ROUND(PSIN(m_fAge*PI*4) * (m_nFrameCount-1));
	CTrueBitmap& bmp = gBitmaps.GetBitmap(m_nBaseID + m_nFrame);

	CFPoint pt;
	map.To(m_pos, pt);

	pt -= m_bboxrad;

	bmp.BlitAlpha(framebuf, 
		(int)pt.x, (int)pt.y, 
		bmp.GetWidth(), bmp.GetHeight(),
		0, 0);
#endif
#endif
}


void Defcon::CBeacon::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& map, FSlateBrush& Brush)
{
	if(this->CollisionCount() > 0)
	{
		return;
	}

	Super::DrawSmall(framebuf, map, Brush);
}
