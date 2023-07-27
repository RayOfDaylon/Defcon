// bmpdisp.cpp
// Implements CBitmapDisplayer game object


#include "bmpdisp.h"
#include "Common/util_core.h"
#include "Common/gamma.h"
#include "Main/mapper.h"
#include "DefconLogging.h"


Defcon::CBitmapDisplayer::CBitmapDisplayer()
{
	UE_LOG(LogGame, Log, TEXT("Instancing bitmap displayer"));

	m_type = ObjType::TEXTURE;
	m_orient.fwd.set(0.0f, 0.0f);
	m_bMortal = true;
	m_bCanBeInjured = false;
	m_fLifespan = 100.0f;
	m_nFrame = 0;
	m_nFrameCount = 0;
	m_nBaseID = 0;
	m_fAge = 0.0f;
	m_fAlphaScale = 0.0f;
	m_fAlphaMin = 1.0f;
}

Defcon::CBitmapDisplayer::~CBitmapDisplayer()
{
	/*if(Sprite)
	{
		UE_LOG(LogGame, Log, TEXT("Uninstalling bitmap displayer widget"));
		Daylon::Uninstall(Sprite);
	}*/
}

#ifdef _DEBUG
const char* Defcon::CBitmapDisplayer::GetClassname() const
{
	static char* psz = "Texture";
	return psz;
}
#endif

void Defcon::CBitmapDisplayer::InitBitmapDisplayer
(
	size_t base, 
	size_t count, 
	float  lifetime,
	float  fAlphaScale,
	float  fAlphaMin
)
{
	m_nBaseID = base;
	m_nFrameCount = count;
	m_fLifespan = m_fOrgLifespan = lifetime;
	m_fAlphaScale = fAlphaScale;
	m_fAlphaMin = fAlphaMin;
	//CTrueBitmap& bmp = gBitmaps.GetBitmap(m_nBaseID);
	//m_bboxrad.set((float)bmp.GetWidth()/2, (float)bmp.GetHeight()/2);
}


void Defcon::CBitmapDisplayer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& map)
{
#if 0
	if(m_nFrame >= m_nFrameCount)
	{
		m_fLifespan = 0.0f;
		return;
	}

	m_nFrame = (int)(m_fAge / m_fOrgLifespan * (m_nFrameCount-1) + 0.5f);
	CTrueBitmap& bmp = gBitmaps.GetBitmap(m_nBaseID + m_nFrame);

	CFPoint pt;
	map.To(m_pos, pt);

	if(bmp.m_bHasAlpha)
	{
		bmp.BlitAlphaBrighten(framebuf, 
			(int)pt.x, (int)pt.y, 
			bmp.GetWidth(), bmp.GetHeight(),
			0, 0, FRAND * m_fAlphaScale + m_fAlphaMin);
	}
	else
		bmp.Blit(framebuf, 
			(int)pt.x, (int)pt.y, 
			bmp.GetWidth(), bmp.GetHeight(),
			0, 0);
#endif
}


void Defcon::CBitmapDisplayer::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& map, FSlateBrush&)
{
}


void Defcon::CBitmapDisplayer::Move(float fElapsedTime)
{
	m_fAge += fElapsedTime;
	m_pos.muladd(m_orient.fwd, fElapsedTime);
}
