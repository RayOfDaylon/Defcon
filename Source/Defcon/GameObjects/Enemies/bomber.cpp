/*
	bomber.cpp
	Bomber type for Defcon game.
	Copyright 2003-2004 Daylon Graphics Ltd.

	rcg		jun 20/04	Fixed bug where ascended lander
						was able to keep itself and its
						human alive by resettng their
						lifespan to 0.1 every frame.
*/


#include "bomber.h"

#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"

#include "GameObjects/obj_types.h"
#include "GameObjects/player.h"

#include "Arenas/DefconPlayViewBase.h"

#pragma optimize("", off)

#define SPLINE_WIDTH_MIN   1000.0f
#define SPLINE_WIDTH_MAX   1500.0f
#define SECONDS_PER_PATH_MIN   1.0f
#define SECONDS_PER_PATH_MAX   1.5f

// -------------------------------------------------

Defcon::CBomber::CBomber()
	:
	m_fLayingMines(0.0f),
	m_yoff(0.0f),
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	m_parentType = m_type;
	m_type = ObjType::BOMBER;
	m_pointValue = BOMBER_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = MakeColorFromComponents(64, 0, 255);
	
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = FRAND * 0.5f + 1.0f;

	CreateSprite(m_type);
	const auto& SpriteInfo = GameObjectResources.Get(m_type);
	m_bboxrad.set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);

	m_secondsPerPath = Daylon::FRandRange(SECONDS_PER_PATH_MIN, SECONDS_PER_PATH_MAX);
}


#ifdef _DEBUG
const char* Defcon::CBomber::GetClassname() const
{
	static char* psz = "Bomber";
	return psz;
};
#endif




void Defcon::CBomber::OnFinishedCreating()
{
	Super::OnFinishedCreating();

	if(m_orient.fwd.x < 0)
	{
		// Use left-facing texture atlas.
		const auto& SpriteInfo = GameObjectResources.Get(ObjType::BOMBER_LEFT);
		Sprite->SetAtlas(SpriteInfo.Atlas->Atlas);
	}


	m_posPrev = m_pos;

	float VDir = SBRAND;

	// Make a spline we will march along

	m_currentPath.m_pt[0] = m_pos;

	m_currentPath.m_pt[3] = m_pos + CFPoint(m_orient.fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(m_arenasize.y / 5, m_arenasize.y / 2));
	m_currentPath.m_pt[3].y = CLAMP(m_currentPath.m_pt[3].y, 0.0f, m_arenasize.y);

	// If we make the control points meet up x-wise, the curve will mimic a sine wave.
	m_currentPath.m_pt[1] = CFPoint((m_pos.x + m_currentPath.m_pt[3].x) / 2, m_pos.y); 
	m_currentPath.m_pt[2] = CFPoint(m_currentPath.m_pt[1].x, m_currentPath.m_pt[3].y); 
}


void Defcon::CBomber::Move(float fTime)
{
	// Move in slightly perturbed sine wave pattern.

	// Our explosions looked bad near the world origin because
	// although we don't wrap m_pos.x here, it does get wrapped 
	// during object processing right after Move(), and then 
	// our default m_inertia = m_pos, m_pos = new pos,  m_inertia = m_pos - m_inertia fails because 
	// we do not increment m_pos, we compute it fresh from our current path which can be an unwrapped version.
	// This was fixed by tracking m_pos into m_posPrev and basing inertia on their difference,
	// thus keeping the computation always in unwrapped space.

	CEnemy::Move(fTime);

	if(gpArena->GetPlayerShip().IsAlive())
	{
		if(m_fLayingMines > 0.0f)
		{
			m_fLayingMines -= fTime;
			if(FRAND < 0.2f)
			{
				gpArena->LayMine(*this, m_pos, 1, 1);
			}
		}
		else if(
			FRAND <= 0.01f 
			&& this->CanBeInjured()
			&& gpArena->IsPointVisible(m_pos)
			&& SGN(m_orient.fwd.x) == SGN(gpArena->GetPlayerShip().m_orient.fwd.x))
		{
			m_fLayingMines = FRAND * .15f + 0.33f;
		}
	}



	float T = m_pathTravelTime / m_secondsPerPath;

	T = CLAMP(T, 0.0f, 1.0f);

	m_currentPath.CalcPt(T, m_pos);

	m_pathTravelTime += fTime;

	if(m_pathTravelTime > m_secondsPerPath)
	{
		do { m_pathTravelTime -= m_secondsPerPath; } while(m_pathTravelTime > m_secondsPerPath);

		// Set up our next flight path.
		// We always toggle vertical direction.
		float VDir = -(SGN(m_currentPath.m_pt[3].y - m_currentPath.m_pt[0].y));

		// Wrap path endpoint so we don't eventually get way off arena.
		m_currentPath.m_pt[3].x = gpArena->WrapX(m_currentPath.m_pt[3].x);

		m_currentPath.m_pt[0] = m_currentPath.m_pt[3];

		m_currentPath.m_pt[3] = m_currentPath.m_pt[0] + CFPoint(m_orient.fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(m_arenasize.y / 5, m_arenasize.y / 2));
		m_currentPath.m_pt[3].y = CLAMP(m_currentPath.m_pt[3].y, 0.0f, m_arenasize.y);

		// If we make the control points meet up x-wise, the curve will mimic a sine wave.
		m_currentPath.m_pt[1] = CFPoint((m_currentPath.m_pt[0].x + m_currentPath.m_pt[3].x) / 2, m_pos.y); 
		m_currentPath.m_pt[2] = CFPoint(m_currentPath.m_pt[1].x, m_currentPath.m_pt[3].y); 
	}

#if 0
	m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff + m_fAge)) * 0.5f * m_screensize.y;
	m_halfwayAltitude = sinf((m_yoff + m_fAge) * 0.6f) * 50 + (0.5f * m_screensize.y);

	//m_pos.x += m_orient.fwd.x * m_xFreq * fTime * m_screensize.x * + (FRAND * .05f + 0.25f);
	//m_pos.y = (float)sin(m_freq * (m_yoff + m_fAge + fTime)) * m_screensize.y * 0.4f + m_screensize.y * 0.5f;

	//const float xinc = m_orient.fwd.x * m_xFreq * fTime * m_screensize.x * + (FRAND * .05f + 0.25f);

	const float y = sinf(m_freq * (m_yoff + m_fAge)) * m_amp + m_halfwayAltitude;

	// Don't start moving until we're close enough to our desired position.
	if(ABS(y - m_orgY) < 10.0f)
	{
		m_pos.x += m_orient.fwd.x * m_xFreq * fTime * m_screensize.x * + (FRAND * .05f + 0.25f);
		m_pos.y = y;
		m_orgY = m_pos.y; // Force all subsequent comparisons to succeed so we keep moving.
	}
#endif

	m_inertia = m_pos - m_posPrev;
	m_posPrev = m_pos;
}


void Defcon::CBomber::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CBomber::GetExplosionColorBase() const
{
	return CGameColors::lightblue;
}


float Defcon::CBomber::GetExplosionMass() const
{
	return 0.75f;
}

#undef SPLINE_WIDTH_MIN
#undef SPLINE_WIDTH_MAX
#undef SECONDS_PER_PATH
#undef SECONDS_PER_PATH_MIN
#undef SECONDS_PER_PATH_MAX

#pragma optimize("", on)
