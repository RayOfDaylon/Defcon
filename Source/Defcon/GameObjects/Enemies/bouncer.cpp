/*
	bouncer.cpp
	Bouncer enemy type for Defcon game.
	Copyright 2009 Daylon Graphics Ltd.

	rcg		dec 11/09	Created.
*/


#include "bouncer.h"





#include "Common/util_color.h"


#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"

#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"

#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"

#include "DefconUtils.h"

// -------------------------------------------------

Defcon::IBouncer::IBouncer()
{
	m_parentType = m_type;
	m_type = ObjType::BOUNCER;
	m_pointValue = BOUNCER_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_WHITE;
	m_fAnimSpeed = FRAND * 0.05f + 0.12f;
	m_orient.fwd.y = 0.0; 
	m_fGravity = FRAND * 10.0f + 5.0f;

	const auto& Info = GameObjectResources.Get(ObjType::FIREBOMBER_TRUE);
	m_bboxrad.set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::IBouncer::~IBouncer()
{
}

#ifdef _DEBUG
const char* Defcon::IBouncer::GetClassname() const
{
	static char* psz = "IBouncer";
	return psz;
};
#endif


void Defcon::IBouncer::Move(float fElapsedTime)
{
	CEnemy::Move(fElapsedTime);
	m_inertia = m_pos;

	// Check if terrain hit.
	WRAP(m_pos.x, 0, gpArena->GetWidth());
	const float h = gpArena->GetTerrainElev(m_pos.x);
	if(m_pos.y < h)
	{
		m_orient.fwd.y *= -1;
		m_pos.y = h;
		m_orient.fwd.x = SGN(m_orient.fwd.x) * (FRAND * 15 + 5);
		// todo: switch sign of x depending on surface normal
		// at terrain impact point.
	}

	// Have gravity pull bouncer down.
	//float speed = ABS(m_orient.fwd.y);
	m_orient.fwd.y -= m_fGravity * fElapsedTime;

	m_pos.muladd(m_orient.fwd, fElapsedTime * 40);


	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
	//	diff *= ABS(sin(m_fAge*PI));
	diff = FMath::Min(diff, 1.5f);

	m_inertia -= m_pos;
	m_inertia *= -1;
}


void Defcon::IBouncer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::IBouncer::Explode(CGameObjectCollection& debris)
{
	// Explode in a thick symmetrical pattern.

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	float fBrightBase;
	this->CreateFireblast(debris, fBrightBase);

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
				pFlak->m_orient.fwd.set(
					sinf(angle), cosf(angle));
				
				pFlak->m_orient.fwd *= (SFRAND*5+6) * (i+2);
				pFlak->m_orient.fwd += m_inertia;

				debris.Add(pFlak);
			}
		}
	}
}

// ---------------------------------------------------------------------------------

Defcon::CBouncer::CBouncer()
{
	m_parentType = m_type;
	m_type = ObjType::BOUNCER_TRUE;

	CreateSprite(ObjType::FIREBOMBER_TRUE);
}


Defcon::CBouncer::~CBouncer()
{
}


void Defcon::CBouncer::Move(float fElapsedTime)
{
	IBouncer::Move(fElapsedTime);

	// todo: Is the firing time based on fps or on time?
	//float diff = (float)UDefconUtils::GetGameInstance(gpArena)->GetScore() / 50000;
	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
		//diff *= (float)(ABS(sin(m_fAge * PI)));
	diff = FMath::Min(diff, 1.5f);

	if(FRAND <= 0.25f * diff
		&& !gpArena->IsEnding()
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(m_pos))
	{
		gpArena->CreateEnemy(ObjType::FIREBALL, m_pos, 0.0f, false, false);
	}
}


// -------------------------------------------------------------------------------

Defcon::CWeakBouncer::CWeakBouncer()
{
	m_parentType = m_type;
	m_type = ObjType::BOUNCER_WEAK;

	CreateSprite(ObjType::FIREBOMBER_WEAK);
}


Defcon::CWeakBouncer::~CWeakBouncer()
{
}


void Defcon::CWeakBouncer::Move(float fElapsedTime)
{
	IBouncer::Move(fElapsedTime);

	// todo: Is the firing time based on fps or on time?
	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
		//diff *= (float)(ABS(sin(m_fAge * PI)));
	diff = FMath::Min(diff, 1.5f);

	if(FRAND <= 0.05f * diff
		&& !gpArena->IsEnding()
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(m_pos))
	{
		gpArena->FireBullet(*this, m_pos, 1, 1);
	}
}
