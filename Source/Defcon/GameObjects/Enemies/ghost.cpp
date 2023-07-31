// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	ghost.cpp
	Ghost enemy type for Defcon game.
*/


#include "ghost.h"

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



Defcon::CGhost::CGhost()
{
	ParentType = Type;
	Type       = EObjType::GHOST;

	PointValue = GHOST_VALUE;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = MakeColorFromComponents(192, 192, 192);
	// Our size is the size of a part x 3.
	AnimSpeed = FRAND * 0.35f + 0.65f;
	m_xFreq = FRAND * 0.5f + 1.0f;
	m_bWaits = BRAND;
	m_numParts = IRAND(4) + 4;
	m_fSpinVel = SFRAND;
	m_fSpinAngle = FRAND;
	m_fSpinVelMax = FRAND * 4.0f + 1.0f;
	DispersalCountdown = 0.0f;

	const auto& Info = GameObjectResources.Get(EObjType::GHOSTPART);
	BboxRadius.Set(Info.Size.X * 1.5f, Info.Size.Y * 1.5f);
}


Defcon::CGhost::~CGhost()
{
}

#ifdef _DEBUG
const char* Defcon::CGhost::GetClassname() const
{
	static char* psz = "Ghost";
	return psz;
};
#endif


void Defcon::CGhost::Move(float fTime)
{
	// Just float around drifting horizontally.

	if(DispersalCountdown > 0.0f)
	{
		DispersalCountdown -= fTime;
		return;
	}

	// We are in normal form (not dispersed).

	CEnemy::Move(fTime);
	Inertia = Position;

	Orientation.Fwd.y = 0.1f * (float)sin(m_freq * (m_yoff + Age)); 


	//float diff = (float)UDefconUtils::GetGameInstance(gpArena)->GetScore() / 50000;
	float diff = (float)gDefconGameInstance->GetScore() / 50000;

	if(m_bWaits)
		diff *= (float)(ABS(sin(Age * PI)));

	diff = FMath::Min(diff, 1.5f);

	if(true/*m_bFiresBullets*/)
	{
		if(FRAND <= 0.05f * diff
			&& this->CanBeInjured()
			&& gpArena->GetPlayerShip().IsAlive()
			&& gpArena->IsPointVisible(Position))
		{
			gpArena->FireBullet(*this, Position, 1, 1);
		}
	}

	m_fSpinVel = 1.0f;//sin(Age * PI * m_fSpinVelMax);
	m_fSpinAngle += (m_fSpinVel * fTime);

	Position.MulAdd(Orientation.Fwd, fTime * 50.0f);
	Inertia = Position - Inertia;

	// See if we need to disperse (player ship got too close).

	CPlayer* pTarget = &gpArena->GetPlayerShip();

	if(pTarget->IsAlive() && !this->MarkedForDeath())
	{
		CFPoint dir;
		const float dist = gpArena->Direction(Position, pTarget->Position, dir);

		if(dist < GHOST_PLAYER_DIST_MIN) // todo: use pref value instead of constant 200
		{
			const float flighttime = FRAND * 1.0f + 0.5f;

			DispersalCountdown = flighttime;

			// We don't have a sprite to hide, and our Draw method takes care of "hiding" us by
			// not rendering if we're dispersed (since the parts will render).
			//Sprite->Hide();

			// While hidden, move us to our reformation spot.
			CFPoint newloc;

			// Choose somewhere else on the screen.
			newloc.x = Position.x + SFRAND * gpArena->GetDisplayWidth() / 1.5f;
			newloc.y = (FRAND * 0.75f + 0.125f) * gpArena->GetHeight();

			// Don't modulate newloc; it will cause wrong path animation if path cross x-origin.
			// Modulation must happen in ghostpart::move.

			for(int32 i = 1; i < m_numParts; i++)
			{
				CGhostPart* p = (CGhostPart*)gpArena->CreateEnemyNow(EObjType::GHOSTPART, m_partLocs[i], false, false);

				p->SetCollisionInjurious(false);
				p->SetFlightDuration(flighttime);
				p->SetFlightPath(Position, newloc);
			}

			// Now move ourselves. Do this last because we use our original Position as the 
			// source position of the dispersal in the above loop.

			Position = newloc;
			Position.x = gpArena->WrapX(Position.x);
		
			gpAudio->OutputSound(ghostflight);
		}
	}
}


void Defcon::CGhost::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	if(DispersalCountdown > 0.0f)
	{
		return;
	}

	m_partLocs[0] = Position;

	float f = (float)fmod(Age, AnimSpeed) / AnimSpeed;
		//(float)Age / AnimSpeed;


	// Draw the parts in a circle around a central part.
	int32 n = m_numParts - 1;
	int32 i;
	for(i = 0; i < n; i++)
	{
		const float t = (float)(TWO_PI * i / n + ((m_fSpinAngle + FRAND*0.1f) * TWO_PI));
		CFPoint pt2((float)cos(t), (float)sin(t));
		float r = (float)(sin((f + FRAND*3) * PI) * 5 + 10);
		BboxRadius.Set(r, r);
		pt2 *= r;
		pt2 += Position;
		m_partLocs[i+1] = pt2;
	}

	for(i = 0; i < m_numParts; i++)
	{
		CFPoint pt;
		mapper.To(m_partLocs[i], pt);
		this->DrawPart(framebuf, pt);
	}
}


void Defcon::CGhost::DrawPart(FPaintArguments& framebuf, const CFPoint& where)
{
	const CFPoint pt = where;

	auto& Info = GameObjectResources.Get(EObjType::GHOSTPART);

	const int w = Info.Size.X;

	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float f = (NumCels - 1) * PSIN(PI * fmod(Age, AnimSpeed) / AnimSpeed);

		const float Usize = 1.0f / NumCels;

		const auto S = Info.Size;
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

		const float F = Usize * ROUND(f);
		FBox2f UVRegion(FVector2f(F, 0.0f), FVector2f(F + Usize, 1.0f));
		Info.Atlas->Atlas.AtlasBrush.SetUVRegion(UVRegion);

		FSlateDrawElement::MakeBox(
			*framebuf.OutDrawElements,
			framebuf.LayerId,
			Geometry.ToPaintGeometry(),
			&Info.Atlas->Atlas.AtlasBrush,
			ESlateDrawEffect::None,
			C_WHITE * framebuf.RenderOpacity);
	}
}


void Defcon::CGhost::OnAboutToDie()
{
	// Release parts.
/*
	for(int i = 0; i < m_numParts; i++)
	{
		gpGame->CreateEnemy(EObjType::GHOSTPART, m_partLocs[i], false, false);
	}
*/
}


void Defcon::CGhost::Explode(CGameObjectCollection& debris)
{
	//CEnemy::Explode(debris);

	bMortal = true;
	Lifespan = 0.0f;
	this->OnAboutToDie();

	// Create an explosion by making
	// several debris objects and 
	// adding them to the debris set.
	int n = (int)(FRAND * 30 + 30);
	float maxsize = FRAND * 5 + 3;

/*
	// Don't use huge particles ever; it just looks silly in the end.
	if(FRAND < .08)
		maxsize = 14;
*/

	// Define which color to make the debris.
	auto cby = this->GetExplosionColorBase();
	maxsize *= this->GetExplosionMass();
	
	if(IRAND(3) == 1)
	{
		cby = EColor::gray;
	}

	bool bDieOff = (FRAND >= 0.25f);
	int i;

	float fBrightBase;
	IGameObject* pFireblast = this->CreateFireblast(debris, fBrightBase);

	for(i = 0; i < n; i++)
	{
		CGlowingFlak* pFlak = new CGlowingFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_fLargestSize = maxsize;
		pFlak->m_bFade = bDieOff;

		pFlak->Position = Position;
		pFlak->Orientation = Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.Fwd *= FRAND * 12.0f + 30.0f;
		// Make the particle have a velocity vector
		// as if it were standing still.
		float speed = FRAND * 180 + 90;


		pFlak->Orientation.Fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		n = (int)(FRAND * 20 + 20);
		maxsize = FRAND * 4 + 8.0f;
		//maxsize = FMath::Min(maxsize, 9.0f);

		if(IRAND(3) == 1)
		{
			cby = EColor::gray;
		}
		else
		{
			cby = this->GetExplosionColorBase();
		}

		for(i = 0; i < n; i++)
		{
			CGlowingFlak* pFlak = new CGlowingFlak;
			pFlak->m_eColorbaseYoung = cby;
			pFlak->m_fLargestSize = maxsize;
			pFlak->m_bFade = bDieOff;

			pFlak->Position = Position;
			pFlak->Orientation = Orientation;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.Set((float)cos(t), (float)sin(t));

			pFlak->Orientation.Fwd = Inertia;

			pFlak->Orientation.Fwd *= FRAND * 12.0f + 30.0f;
			float speed = FRAND * 45 + 22;

			pFlak->Orientation.Fwd.MulAdd(dir, speed);

			debris.Add(pFlak);
		}
	}


#if 1
	cby = EColor::gray;


	for(i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = cby;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->Position = Position;
		pFlak->Orientation = Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.Fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->Orientation.Fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}
#endif
}

// --------------------------------------------------------


Defcon::CGhostPart::CGhostPart()
{
	ParentType = Type;
	Type = EObjType::GHOSTPART;
	PointValue = 0;
	Orientation.Fwd.Set(1.0f, 0.0f);
	RadarColor = C_WHITE;
	AnimSpeed = FRAND * 0.35f + 0.15f;
	bCanBeInjured = false;
	bIsCollisionInjurious = false;

	CreateSprite(Type);
	const auto& Info = GameObjectResources.Get(Type);
	BboxRadius.Set(Info.Size.X / 2, Info.Size.Y / 2);
}


Defcon::CGhostPart::~CGhostPart()
{
}


#ifdef _DEBUG
const char* Defcon::CGhostPart::GetClassname() const
{
	static char* psz = "Ghost_part";
	return psz;
};
#endif


void Defcon::CGhostPart::SetFlightPath(const CFPoint& from, const CFPoint& to)
{
	// from and to must be unmodulated.

	m_path.Pts[0] = m_path.Pts[1] = from;
	m_path.Pts[2] = m_path.Pts[3] = to;

	// Make the control points match their anchor but with a random offset.
	m_path.Pts[1] += CFPoint(SFRAND * IRAND(300), SFRAND * IRAND(300));
	m_path.Pts[2] += CFPoint(SFRAND * IRAND(300), SFRAND * IRAND(300));
}


void Defcon::CGhostPart::Move(float fTime)
{
	// All we have to do is move along our predetermined spline 
	// and at the end, rebuild a ghost.

	CEnemy::Move(fTime);

	// We're moving along a spline, so determine our normalized distance along it.

	if(Age < m_fMaxAge)
	{
		m_path.CalcPt(powf(Age / m_fMaxAge, 0.75f), Position);
		Position.x = gpArena->WrapX(Position.x);
		return;
	}

	this->MarkAsDead();
}


void Defcon::CGhostPart::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}



void Defcon::CGhostPart::Explode(CGameObjectCollection& debris)
{
	// Ghost parts don't explode.
}
