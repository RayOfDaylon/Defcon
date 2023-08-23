// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	bouncer.cpp
	Bouncer enemy type for Defcon game.

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
	Type       = EObjType::BOUNCER;

	PointValue        = BOUNCER_VALUE;
	RadarColor        = C_WHITE;
	AnimSpeed         = FRANDRANGE(0.12f, 0.17f);
	Gravity           = FRANDRANGE(5, 15);
	FiringCountdown   = FRANDRANGE(1.0f, 5.0f);

	Orientation.Fwd.Set(1.0f, 0.0f);

	const auto& Info = GGameObjectResources.Get(EObjType::FIREBOMBER_TRUE);
	BboxRadius = Info.Size / 2;
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


void Defcon::IBouncer::Tick(float DeltaTime)
{
	CEnemy::Tick(DeltaTime);

	Inertia = Position;

	// Check if terrain hit.
	WRAP(Position.x, 0, GArena->GetWidth());
	const float h = GArena->GetTerrainElev(Position.x);

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
	Orientation.Fwd.y -= Gravity * DeltaTime;

	Position.MulAdd(Orientation.Fwd, DeltaTime * 40);

	// See if we need to shoot.

	if(IsOurPositionVisible() && TargetPtr != nullptr)
	{
		FiringCountdown -= DeltaTime;

		if(FiringCountdown <= 0.0f)
		{
			ResetFiringCountdown();
			FireWeapon();
		}
	}

#if 0
	float diff = (float)gDefconGameInstance->GetScore() / 50000;
	//if(m_bWaits)
	//	diff *= ABS(sin(Age*PI));
	diff = FMath::Min(diff, 1.5f);
#endif

	Inertia = Position - Inertia;
}


void Defcon::IBouncer::Explode(CGameObjectCollection& Debris)
{
	// Explode in a thick symmetrical pattern.

	// todo: either use different sprites for bouncers, or consolidate explosion code with firebombers.

	CreateExplosionFireball(EExplosionFireball::BrightBall, Debris);
		
	int32 a, i;
	const float off  = SFRAND * 0.2f;
	const float off2 = SFRAND * 0.2f;

	for(a = 0; a < 8; a++)
	{
		for(i = 0; i < 10; i++)
		{
			CFlak* Flak = new CFlak;
			Flak->ColorbaseYoung = BRAND ? EColor::Gray : EColor::Yellow;

			float largest = FRAND * 6 + 5;
			Flak->LargestSize = MAP(i, 0, 9, largest, 4);
			Flak->bFade = true;

			Flak->Position = Position;

			float angle = MAP(a, 0, 7, 0, 5.5f);
			angle += off + SFRAND * 0.05f;
			Flak->Orientation.Fwd.Set(sinf(angle), cosf(angle));
			
			Flak->Orientation.Fwd *= (SFRAND*15+30) * (i+2);
			Flak->Orientation.Fwd += Inertia;

			Debris.Add(Flak);
		}
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		for(a = 0; a < 8; a++)
		{
			for(i = 0; i < 10; i++)
			{
				CFlak* Flak = new CFlak;
				Flak->ColorbaseYoung = BRAND ? EColor::Gray : EColor::Yellow;

				float largest = FRAND * 6 + 5;
				Flak->LargestSize = MAP(i, 0, 9, largest, 4);
				Flak->bFade = true;

				Flak->Position = Position;

				float angle = MAP(a, 0, 7, 0, 5.5f);
				angle += off2 + SFRAND * 0.05f;
				Flak->Orientation.Fwd.Set(sinf(angle), cosf(angle));
				
				Flak->Orientation.Fwd *= (SFRAND * 5 + 6) * (i + 2);
				Flak->Orientation.Fwd += Inertia;

				Debris.Add(Flak);
			}
		}
	}
}

// ---------------------------------------------------------------------------------

Defcon::CBouncer::CBouncer()
{
	ParentType = Type;
	Type       = EObjType::BOUNCER_TRUE;

	CreateSprite(EObjType::FIREBOMBER_TRUE);
}


Defcon::CBouncer::~CBouncer()
{
}


void Defcon::CBouncer::ResetFiringCountdown()
{
	float T = (float)GDefconGameInstance->GetScore() / 50000;
	T = CLAMP(T, 0.0f, 1.0f);

	FiringCountdown = LERP(2.0f, 0.25f, T) + Daylon::FRandRange(0.0f, 0.2f);
}


void Defcon::CBouncer::FireWeapon()
{
	GArena->CreateEnemy(EObjType::FIREBALL, GetType(), Position, 0.0f, EObjectCreationFlags::EnemyPart);
}


// -------------------------------------------------------------------------------

Defcon::CWeakBouncer::CWeakBouncer()
{
	ParentType = Type;
	Type       = EObjType::BOUNCER_WEAK;

	CreateSprite(EObjType::FIREBOMBER_WEAK);
}


Defcon::CWeakBouncer::~CWeakBouncer()
{
}


void Defcon::CWeakBouncer::ResetFiringCountdown()
{
	float T = (float)GDefconGameInstance->GetScore() / 50000;
	T = CLAMP(T, 0.0f, 1.0f);

	FiringCountdown = LERP(2.0f, 0.1f, T) + Daylon::FRandRange(0.0f, 0.2f);
}


void Defcon::CWeakBouncer::FireWeapon()
{
	GArena->FireBullet(*this, Position, 1, 1);
}

