// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

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
#include "Globals/GameObjectResources.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconUtils.h"

// -------------------------------------------------

Defcon::IBouncer::IBouncer()
{
	ParentType = Type;
	Type = EObjType::BOUNCER;
	PointValue = BOUNCER_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_WHITE;
	AnimSpeed = FRAND * 0.05f + 0.12f;
	Orientation.Fwd.y = 0.0; 
	m_fGravity = FRAND * 10.0f + 5.0f;

	const auto& Info = GameObjectResources.Get(EObjType::FIREBOMBER_TRUE);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
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
	Inertia = Position;

	// Check if terrain hit.
	WRAP(Position.x, 0, gpArena->GetWidth());
	const float h = gpArena->GetTerrainElev(Position.x);
	if(Position.y < h)
	{
		Orientation.Fwd.y *= -1;
		Position.y = h;
		Orientation.Fwd.x = SGN(Orientation.Fwd.x) * (FRAND * 15 + 5);
		// todo: switch sign of x depending on surface normal
		// at terrain impact point.
	}

	// Have gravity pull bouncer down.
	//float speed = ABS(Orientation.Fwd.y);
	Orientation.Fwd.y -= m_fGravity * fElapsedTime;

	Position.MulAdd(Orientation.Fwd, fElapsedTime * 40);


	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
	//	diff *= ABS(sin(Age*PI));
	diff = FMath::Min(diff, 1.5f);

	Inertia = Position - Inertia;
}


void Defcon::IBouncer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::IBouncer::Explode(CGameObjectCollection& debris)
{
	// Explode in a thick symmetrical pattern.

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	float fBrightBase;
	this->CreateFireblast(debris, fBrightBase);

	int32 a, i;
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

			pFlak->Position = Position;

			float angle = MAP(a, 0, 7, 0, 5.5f);
			angle += off + SFRAND * 0.05f;
			pFlak->Orientation.Fwd.Set(sinf(angle), cosf(angle));
			
			pFlak->Orientation.Fwd *= (SFRAND*15+30) * (i+2);
			pFlak->Orientation.Fwd += Inertia;

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

				pFlak->Position = Position;

				float angle = MAP(a, 0, 7, 0, 5.5f);
				angle += off2 + SFRAND * 0.05f;
				pFlak->Orientation.Fwd.Set(
					sinf(angle), cosf(angle));
				
				pFlak->Orientation.Fwd *= (SFRAND*5+6) * (i+2);
				pFlak->Orientation.Fwd += Inertia;

				debris.Add(pFlak);
			}
		}
	}
}

// ---------------------------------------------------------------------------------

Defcon::CBouncer::CBouncer()
{
	ParentType = Type;
	Type = EObjType::BOUNCER_TRUE;

	CreateSprite(EObjType::FIREBOMBER_TRUE);
}


Defcon::CBouncer::~CBouncer()
{
}


void Defcon::CBouncer::Move(float fElapsedTime)
{
	IBouncer::Move(fElapsedTime);

	// todo: Is the firing time based on fps or on time?
	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
		//diff *= (float)(ABS(sin(Age * PI)));
	diff = FMath::Min(diff, 1.5f);

	if(FRAND <= 0.25f * diff
		&& !gpArena->IsEnding()
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(Position))
	{
		gpArena->CreateEnemy(EObjType::FIREBALL, Position, 0.0f, false, false);
	}
}


// -------------------------------------------------------------------------------

Defcon::CWeakBouncer::CWeakBouncer()
{
	ParentType = Type;
	Type = EObjType::BOUNCER_WEAK;

	CreateSprite(EObjType::FIREBOMBER_WEAK);
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
		//diff *= (float)(ABS(sin(Age * PI)));
	diff = FMath::Min(diff, 1.5f);

	if(FRAND <= 0.05f * diff
		&& !gpArena->IsEnding()
		&& this->CanBeInjured()
		&& gpArena->GetPlayerShip().IsAlive()
		&& gpArena->IsPointVisible(Position))
	{
		gpArena->FireBullet(*this, Position, 1, 1);
	}
}
