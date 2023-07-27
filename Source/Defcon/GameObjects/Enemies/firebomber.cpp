/*
	firebomber.cpp
	Firebomber enemy type for Defcon game.
	Copyright 2004 Daylon Graphics Ltd.
*/


#include "firebomber.h"




#include "DefconUtils.h"

#include "Common/util_color.h"


#include "Globals/_sound.h"

#include "Globals/prefs.h"

#include "Globals/GameColors.h"

#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"

#include "Arenas/DefconPlayViewBase.h"
#include "Globals/GameObjectResources.h"

/*
	Firebomber movement is composed of travel for a short period
	along a straight line (a diagonal one), after which the travel 
	direction is reversed horizontally but may or may not reverse
	vertically. The distance is always random.

*/


// -------------------------------------------------

Defcon::IFirebomber::IFirebomber()
{
	m_parentType = m_type;
	m_type = ObjType::FIREBOMBER;
	m_pointValue = FIREBOMBER_VALUE;

	m_smallColor = C_WHITE;
	
	m_fAnimSpeed = FRAND * 0.05f + 0.12f;
	m_bAbsSin = false;
	m_sgn = 1;
	m_bWaits = BRAND;

	m_firingCountdown = 2.0f;

	m_travelCountdown = 1.0f;

	m_orient.fwd.set(SBRAND, SBRAND);
	m_ourInertia = m_orient.fwd * Daylon::FRandRange(FIREBOMBER_SPEED_MIN, FIREBOMBER_SPEED_MAX);

	const auto& Info = GameObjectResources.Get(ObjType::FIREBOMBER_TRUE);
	m_bboxrad.set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::IFirebomber::~IFirebomber()
{
}

#ifdef _DEBUG
const char* Defcon::IFirebomber::GetClassname() const
{
	static char* psz = "Firebomber";
	return psz;
};
#endif


void Defcon::IFirebomber::Move(float fTime)
{
	// Just float around drifting horizontally.

	Super::Move(fTime);
	m_inertia = m_pos;

	m_travelCountdown -= fTime;

	if(m_travelCountdown <= 0.0f)
	{
		// We've finished traveling, so define a new direction and travel length.
		m_orient.fwd.set(SBRAND, SBRAND);
		m_ourInertia      = m_orient.fwd * Daylon::FRandRange(FIREBOMBER_SPEED_MIN, FIREBOMBER_SPEED_MAX);

		if(IRAND(3) == 1)
		{
			m_ourInertia *= 0.5f;
		}

		m_travelCountdown = Daylon::FRandRange(FIREBOMBER_TRAVEL_TIME_MIN, FIREBOMBER_TRAVEL_TIME_MAX);
	}

	m_pos += m_ourInertia * fTime;


	WRAP(m_pos.y, 0, m_screensize.y);

	if(gpArena->IsPointVisible(m_pos))
	{
		m_firingCountdown -= fTime;
	}

	m_inertia = m_pos - m_inertia;
}


void Defcon::IFirebomber::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
#if 0
	CEnemy::Draw(framebuf, mapper);
	if(this->IsMaterializing())
		return;

	CFPoint pt;
	mapper.To(m_pos, pt);


	float f = (float)fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed;
	//f = (float)cos(f * PI) + 1.0f;
	f *= 3;
	
	CTrueBitmap& bmp = gBitmaps.GetBitmap(
		(m_bFiresBullets 
		? CBitmaps::firebomber0
		: CBitmaps::firebomber_true0)
		+ ROUND(f));
	int w = bmp.GetWidth();
	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		pt.sub(CFPoint((float)w/2,
					(float)bmp.GetHeight()/2));
		bmp.BlitAlphaBrighten(
			framebuf, ROUND(pt.x), ROUND(pt.y), 
			w, bmp.GetHeight(), 
			0, 0, m_fBrightness * (FRAND * .10f + 0.9f));
	}
#endif
}



void Defcon::IFirebomber::Explode(CGameObjectCollection& debris)
{
	// Explode in a thick symmetrical pattern.

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	float fBrightBase;
	this->CreateFireball(debris, fBrightBase);

	size_t a, i;
	const float off = SFRAND * 0.2f;
	const float off2 = SFRAND * 0.2f;

	for(a = 0; a < 8; a++)
	{
		for(i = 0; i < 10; i++)
		{
			CFlak* pFlak = new CFlak;
			pFlak->m_eColorbaseYoung = BRAND ? CGameColors::gray : CGameColors::yellow;

			float largest = FRAND * 6 + 5;
			pFlak->m_fLargestSize = MAP(i, 0, 9, largest, 4);
			pFlak->m_bFade = true;

			pFlak->m_pos = m_pos;

			float angle = MAP(a, 0, 7, 0, 5.5f);
			angle += off + SFRAND * 0.05f;
			pFlak->m_orient.fwd.set(sinf(angle), cosf(angle));
			
			pFlak->m_orient.fwd *= (SFRAND*15+30) * (i+2);
			pFlak->m_orient.fwd += m_inertia;

			debris.Add(pFlak);
		}
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		for(a = 0; a < 8; a++)
		{
			for(i = 0; i < 10; i++)
			{
				CFlak* pFlak = new CFlak;
				pFlak->m_eColorbaseYoung = BRAND ? CGameColors::gray : CGameColors::yellow;

				float largest = FRAND * 6 + 5;
				pFlak->m_fLargestSize = MAP(i, 0, 9, largest, 4);
				pFlak->m_bFade = true;

				pFlak->m_pos = m_pos;

				float angle = MAP(a, 0, 7, 0, 5.5f);
				angle += off2 + SFRAND * 0.05f;
				pFlak->m_orient.fwd.set(sinf(angle), cosf(angle));
				
				pFlak->m_orient.fwd *= (SFRAND*5+6) * (i+2);
				pFlak->m_orient.fwd += m_inertia;

				debris.Add(pFlak);
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------

Defcon::CFirebomber::CFirebomber()
{
	m_parentType = m_type;
	m_type = ObjType::FIREBOMBER_TRUE;

	CreateSprite(m_type);
}


Defcon::CFirebomber::~CFirebomber()
{
}


void Defcon::CFirebomber::Move(float fTime)
{
	Super::Move(fTime);

	if(!(this->CanBeInjured() && gpArena->GetPlayerShip().IsAlive() && gpArena->IsPointVisible(m_pos)))
	{
		return;
	}

	if(m_firingCountdown <= 0.0f)
	{
		gpArena->CreateEnemy(ObjType::FIREBALL, m_pos, 0.0f, false, false);

		// The time to fire goes down as the player XP increases.

		//m_firingCountdown = 2.0f;

		float XP = (float)gDefconGameInstance->GetScore();

		float T = NORM_(XP, 1000.0f, 50000.f);
		T = CLAMP(T, 0.0f, 1.0f);

		m_firingCountdown = LERP(2.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
	}
}


// ------------------------------------------------------------------------------------------------------

Defcon::CWeakFirebomber::~CWeakFirebomber()
{
}


Defcon::CWeakFirebomber::CWeakFirebomber()
{
	m_parentType = m_type;
	m_type = ObjType::FIREBOMBER_WEAK;

	CreateSprite(m_type);
}


void Defcon::CWeakFirebomber::Move(float fTime)
{
	Super::Move(fTime);

	//float diff = (float)UDefconUtils::GetGameInstance(gpArena)->GetScore() / 50000;
	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	if(m_bWaits)
		diff *= (float)(ABS(sin(m_fAge * PI)));
	diff = FMath::Min(diff, 1.5f);

	if(FRAND <= 0.05f * diff
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(m_pos))
	{
		gpArena->FireBullet(*this, m_pos, 1, 1);
	}
}
