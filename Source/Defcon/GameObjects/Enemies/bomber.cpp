// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	bomber.cpp
	Bomber type for Defcon game.
*/


#include "bomber.h"

#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"

#include "GameObjects/obj_types.h"
#include "GameObjects/player.h"

#include "Arenas/DefconPlayViewBase.h"

#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


#define SPLINE_WIDTH_MIN   1000.0f
#define SPLINE_WIDTH_MAX   1500.0f
#define SECONDS_PER_PATH_MIN   1.0f
#define SECONDS_PER_PATH_MAX   1.5f



Defcon::CBomber::CBomber()
	:
	m_fLayingMines(0.0f),
	m_yoff(0.0f),
	m_freq(2.0f),
	m_xFreq(1.0f)
{
	ParentType = Type;
	Type = EObjType::BOMBER;
	PointValue = BOMBER_VALUE;
	Orientation.fwd.Set(1.0f, 0.0f);
	RadarColor = MakeColorFromComponents(64, 0, 255);
	
	AnimSpeed = FRAND * 0.35f + 0.15f;
	m_xFreq = FRAND * 0.5f + 1.0f;

	CreateSprite(Type);
	const auto& SpriteInfo = GameObjectResources.Get(Type);
	BboxRadius.Set(SpriteInfo.Size.X / 2, SpriteInfo.Size.Y / 2);

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

	if(Orientation.fwd.x < 0)
	{
		// Use left-facing texture atlas.
		const auto& SpriteInfo = GameObjectResources.Get(EObjType::BOMBER_LEFT);
		Sprite->SetAtlas(SpriteInfo.Atlas->Atlas);
	}


	m_posPrev = Position;

	float VDir = SBRAND;

	// Make a spline we will march along

	m_currentPath.m_pt[0] = Position;

	m_currentPath.m_pt[3] = Position + CFPoint(Orientation.fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(ArenaSize.y / 5, ArenaSize.y / 2));
	m_currentPath.m_pt[3].y = CLAMP(m_currentPath.m_pt[3].y, 0.0f, ArenaSize.y);

	// If we make the control points meet up x-wise, the curve will mimic a sine wave.
	m_currentPath.m_pt[1] = CFPoint((Position.x + m_currentPath.m_pt[3].x) / 2, Position.y); 
	m_currentPath.m_pt[2] = CFPoint(m_currentPath.m_pt[1].x, m_currentPath.m_pt[3].y); 
}


void Defcon::CBomber::Move(float fTime)
{
	// Move in slightly perturbed sine wave pattern.

	// Our explosions looked bad near the world origin because
	// although we don't wrap Position.x here, it does get wrapped 
	// during object processing right after Move(), and then 
	// our default Inertia = Position, Position = new pos,  Inertia = Position - Inertia fails because 
	// we do not increment Position, we compute it fresh from our current path which can be an unwrapped version.
	// This was fixed by tracking Position into m_posPrev and basing inertia on their difference,
	// thus keeping the computation always in unwrapped space.

	CEnemy::Move(fTime);

	if(gpArena->GetPlayerShip().IsAlive())
	{
		if(m_fLayingMines > 0.0f)
		{
			m_fLayingMines -= fTime;
			if(FRAND < 0.2f)
			{
				gpArena->LayMine(*this, Position, 1, 1);
			}
		}
		else if(
			FRAND <= 0.01f 
			&& this->CanBeInjured()
			&& gpArena->IsPointVisible(Position)
			&& SGN(Orientation.fwd.x) == SGN(gpArena->GetPlayerShip().Orientation.fwd.x))
		{
			m_fLayingMines = FRAND * .15f + 0.33f;
		}
	}



	float T = m_pathTravelTime / m_secondsPerPath;

	T = CLAMP(T, 0.0f, 1.0f);

	m_currentPath.CalcPt(T, Position);

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

		m_currentPath.m_pt[3] = m_currentPath.m_pt[0] + CFPoint(Orientation.fwd.x * Daylon::FRandRange(SPLINE_WIDTH_MIN, SPLINE_WIDTH_MAX), VDir * (float)Daylon::FRandRange(ArenaSize.y / 5, ArenaSize.y / 2));
		m_currentPath.m_pt[3].y = CLAMP(m_currentPath.m_pt[3].y, 0.0f, ArenaSize.y);

		// If we make the control points meet up x-wise, the curve will mimic a sine wave.
		m_currentPath.m_pt[1] = CFPoint((m_currentPath.m_pt[0].x + m_currentPath.m_pt[3].x) / 2, Position.y); 
		m_currentPath.m_pt[2] = CFPoint(m_currentPath.m_pt[1].x, m_currentPath.m_pt[3].y); 
	}

#if 0
	m_amp = LERP(0.33f, 1.0f, PSIN(m_yoff + Age)) * 0.5f * ScreenSize.y;
	m_halfwayAltitude = sinf((m_yoff + Age) * 0.6f) * 50 + (0.5f * ScreenSize.y);

	//Position.x += Orientation.fwd.x * m_xFreq * fTime * ScreenSize.x * + (FRAND * .05f + 0.25f);
	//Position.y = (float)sin(m_freq * (m_yoff + Age + fTime)) * ScreenSize.y * 0.4f + ScreenSize.y * 0.5f;

	//const float xinc = Orientation.fwd.x * m_xFreq * fTime * ScreenSize.x * + (FRAND * .05f + 0.25f);

	const float y = sinf(m_freq * (m_yoff + Age)) * m_amp + m_halfwayAltitude;

	// Don't start moving until we're close enough to our desired position.
	if(ABS(y - m_orgY) < 10.0f)
	{
		Position.x += Orientation.fwd.x * m_xFreq * fTime * ScreenSize.x * + (FRAND * .05f + 0.25f);
		Position.y = y;
		m_orgY = Position.y; // Force all subsequent comparisons to succeed so we keep moving.
	}
#endif

	Inertia = Position - m_posPrev;
	m_posPrev = Position;
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

#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
