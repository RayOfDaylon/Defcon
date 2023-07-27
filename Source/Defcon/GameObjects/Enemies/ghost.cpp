/*
	ghost.cpp
	Ghost enemy type for Defcon game.
	Copyright 2004 Daylon Graphics Ltd.
*/


#include "ghost.h"





#include "Common/util_color.h"


#include "Globals/_sound.h"

#include "Globals/prefs.h"

#include "Globals/GameColors.h"

#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"


#include "Arenas/DefconPlayViewBase.h"
#include "Globals/GameObjectResources.h"

#include "DefconUtils.h"

// -------------------------------------------------

Defcon::CGhost::CGhost()
{
	m_parentType = m_type;
	m_type       = ObjType::GHOST;

	m_pointValue = GHOST_VALUE;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = MakeColorFromComponents(192, 192, 192);
	// Our size is the size of a part x 3.
	m_fAnimSpeed = FRAND * 0.35f + 0.65f;
	m_xFreq = FRAND * 0.5f + 1.0f;
	m_bWaits = BRAND;
	m_numParts = IRAND(4) + 4;
	m_fSpinVel = SFRAND;
	m_fSpinAngle = FRAND;
	m_fSpinVelMax = FRAND * 4.0f + 1.0f;
	DispersalCountdown = 0.0f;

	const auto& Info = GameObjectResources.Get(ObjType::GHOSTPART);
	m_bboxrad.set(Info.Size.X * 1.5f, Info.Size.Y * 1.5f);
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

	if(DispersalCountdown <= 0.0f)
	{
		// We are in normal form (not dispersed).

		CEnemy::Move(fTime);
		m_inertia = m_pos;

		m_orient.fwd.y = 0.1f * (float)sin(m_freq * (m_yoff + m_fAge)); 


		//float diff = (float)UDefconUtils::GetGameInstance(gpArena)->GetScore() / 50000;
		float diff = (float)gDefconGameInstance->GetScore() / 50000;

		if(m_bWaits)
			diff *= (float)(ABS(sin(m_fAge * PI)));

		diff = FMath::Min(diff, 1.5f);

		if(true/*m_bFiresBullets*/)
		{
			if(FRAND <= 0.05f * diff
				&& this->CanBeInjured()
				&& gpArena->GetPlayerShip().IsAlive()
				&& gpArena->IsPointVisible(m_pos))
				{
					gpArena->FireBullet(*this, m_pos, 1, 1);
				}
		}

		m_fSpinVel = 1.0f;//sin(m_fAge * PI * m_fSpinVelMax);
		m_fSpinAngle += (m_fSpinVel * fTime);

		m_pos.muladd(m_orient.fwd, fTime * 50.0f);
		m_inertia = m_pos - m_inertia;

		// See if we need to disperse (player ship got too close).

		CPlayer* pTarget = &gpArena->GetPlayerShip();

		if(pTarget->IsAlive() && !this->MarkedForDeath())
		{
			CFPoint dir;
			const float dist = gpArena->Direction(m_pos, pTarget->m_pos, dir);

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
				newloc.x = m_pos.x + SFRAND * gpArena->GetDisplayWidth() / 1.5f;
				newloc.y = (FRAND * 0.75f + 0.125f) * gpArena->GetHeight();

				// Don't modulate newloc; it will cause wrong path animation if path cross x-origin.
				// Modulation must happen in ghostpart::move.

				for(int32 i = 1; i < m_numParts; i++)
				{
					CGhostPart* p = (CGhostPart*)gpArena->CreateEnemyNow(ObjType::GHOSTPART, m_partLocs[i], false, false);

					p->SetCollisionInjurious(false);
					p->SetFlightDuration(flighttime);
					p->SetFlightPath(m_pos, newloc);
				}

				// Now move ourselves. Do this last because we use our original m_pos as the 
				// source position of the dispersal in the above loop.

				m_pos = newloc;
				m_pos.x = gpArena->WrapX(m_pos.x);
		
				gpAudio->OutputSound(snd_ghostflight);
			}
		}
	}
	else
	{
		// We're dispersed, eventually to reform.

		DispersalCountdown -= fTime;
/*
		if(DispersalCountdown <= 0.0f)
		{
			// We've finished reforming, the parts will destroy themselves.
			//Sprite->Show();
		}
		else
		{
			// Still dispersed, so keep waiting.
		}
*/
	}

#if 0
	CPlayer* pTarget = &gpArena->GetPlayerShip();

	if(pTarget->IsAlive() && !this->MarkedForDeath())
	{
		CFPoint dir;
		const float dist = gpArena->Direction(m_pos, pTarget->m_pos, dir);

		if(dist < GHOST_PLAYER_DIST_MIN) // todo: use pref value instead of constant 200
		{
			// Fragment into parts that will converge to reform us later

			const float flighttime = FRAND * 1.0f + 0.5f;

			DispersalCountdown = flighttime;

			Sprite->Hide();

			// todo: use IsDispersed member instead of dying and birthing a CGhostPrimaryPart

			CGhostPrimaryPart* pE = (CGhostPrimaryPart*)gpArena->CreateEnemyNow(ObjType::GHOSTPARTPRIMARY, m_partLocs[0], false, false);

			CFPoint newloc;

			// Choose somewhere else on the screen.
			newloc.x = m_pos.x + SFRAND * gpArena->GetDisplayWidth() / 1.5f;
			newloc.y = (FRAND * 0.75f + 0.125f) * gpArena->GetHeight();

			// Don't modulate newloc; it will cause wrong path animation if path cross x-origin.
			// Modulation must happen in ghostpart::move.
			
			pE->SetCollisionInjurious(false);
			pE->SetNewLocation(newloc);
			pE->SetFlightDuration(flighttime);
			pE->SetFlightPath(m_pos, newloc);

			for(size_t i = 1; i < m_numParts; i++)
			{
				CGhostPart* p = (CGhostPart*)gpArena->CreateEnemyNow(ObjType::GHOSTPART, m_partLocs[i], false, false);

				p->SetCollisionInjurious(false);
				p->SetFlightDuration(flighttime);
				p->SetFlightPath(m_pos, newloc);
			}
		
			this->MarkAsDead();

			gpAudio->OutputSound(snd_ghostflight);
		}
	}
#endif
}


void Defcon::CGhost::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	CEnemy::Draw(framebuf, mapper);

	if(DispersalCountdown > 0.0f)
	{
		return;
	}

	m_partLocs[0] = m_pos;

	float f = (float)fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed;
		//(float)m_fAge / m_fAnimSpeed;


	// Draw the parts in a circle around a central part.
	size_t n = m_numParts - 1;
	size_t i;
	for(i = 0; i < n; i++)
	{
		const float t = (float)(TWO_PI * i / n + ((m_fSpinAngle + FRAND*0.1f) * TWO_PI));
		CFPoint pt2((float)cos(t), (float)sin(t));
		float r = (float)(sin((f + FRAND*3) * PI) * 5 + 10);
		m_bboxrad.set(r, r);
		pt2 *= r;
		pt2 += m_pos;
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

	auto& Info = GameObjectResources.Get(ObjType::GHOSTPART);

	const int w = Info.Size.X;

	if(pt.x >= -w && pt.x <= framebuf.GetWidth() + w)
	{
		const int32 NumCels = Info.Atlas->Atlas.NumCels;
		const float f = (NumCels - 1) * PSIN(PI * fmod(m_fAge, m_fAnimSpeed) / m_fAnimSpeed);

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
			C_WHITE * framebuf.RenderOpacity * framebuf.InWidgetStyle->GetColorAndOpacityTint().A);
	}
}


void Defcon::CGhost::OnAboutToDie()
{
	// Release parts.
/*
	for(int i = 0; i < m_numParts; i++)
	{
		gpGame->CreateEnemy(ObjType::GHOSTPART, m_partLocs[i], false, false);
	}
*/
}


void Defcon::CGhost::Explode(CGameObjectCollection& debris)
{
	//CEnemy::Explode(debris);

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	// Create an explosion by making
	// several debris objects and 
	// adding them to the debris set.
	int n = (int)(FRAND * 30 + 30);
	float maxsize = FRAND * 5 + 3;

/*
	// Don't use huge particles ever; it 
	// just looks silly in the end.
	if(FRAND < .08)
		maxsize = 14;
*/

	// Define which color to make the debris.
	int cby = this->GetExplosionColorBase();
	maxsize *= this->GetExplosionMass();
	
	if(IRAND(3) == 1)
		cby = CGameColors::gray;

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

		pFlak->m_pos = m_pos;
		pFlak->m_orient = m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
		// Make the particle have a velocity vector
		// as if it were standing still.
		float speed = FRAND * 180 + 90;


		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		n = (int)(FRAND * 20 + 20);
		maxsize = FRAND * 4 + 8.0f;
		//maxsize = FMath::Min(maxsize, 9.0f);

		if(IRAND(3) == 1)
			cby = CGameColors::gray;
		else
			cby = this->GetExplosionColorBase();

		for(i = 0; i < n; i++)
		{
			CGlowingFlak* pFlak = new CGlowingFlak;
			pFlak->m_eColorbaseYoung = cby;
			pFlak->m_fLargestSize = maxsize;
			pFlak->m_bFade = bDieOff;
			pFlak->m_bDrawSmall = false;

			pFlak->m_pos = m_pos;
			pFlak->m_orient = m_orient;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.set((float)cos(t), (float)sin(t));

			pFlak->m_orient.fwd = m_inertia;

			pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
			float speed = FRAND * 45 + 22;

			pFlak->m_orient.fwd.muladd(dir, speed);

			debris.Add(pFlak);
		}
	}


#if 1
	cby = CGameColors::gray;


	for(i = 0; i < 20; i++)
	{
		CFlak* pFlak = new CFlak;
		pFlak->m_eColorbaseYoung = cby;
		pFlak->m_eColorbaseOld = cby;
		pFlak->m_bCold = true;
		pFlak->m_fLargestSize = 4;
		pFlak->m_bFade = true;//bDieOff;

		pFlak->m_pos = m_pos;
		pFlak->m_orient = m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 20.0f;
		//ndir *= FRAND * 0.4f + 0.2f;
		float speed = FRAND * 30 + 110;

		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}
#endif
}

// --------------------------------------------------------


Defcon::CGhostPart::CGhostPart()
{
	m_parentType = m_type;
	m_type = ObjType::GHOSTPART;
	m_pointValue = 0;
	m_orient.fwd.set(1.0f, 0.0f);
	m_smallColor = C_WHITE;
	m_fAnimSpeed = FRAND * 0.35f + 0.15f;
	m_bCanBeInjured = false;
	m_bIsCollisionInjurious = false;

	CreateSprite(m_type);
	const auto& Info = GameObjectResources.Get(m_type);
	m_bboxrad.set(Info.Size.X / 2, Info.Size.Y / 2);
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

	m_path.m_pt[0] = m_path.m_pt[1] = from;
	m_path.m_pt[2] = m_path.m_pt[3] = to;

	// Make the control points match their anchor but with a random offset.
	m_path.m_pt[1] += CFPoint(SFRAND * IRAND(300), SFRAND * IRAND(300));
	m_path.m_pt[2] += CFPoint(SFRAND * IRAND(300), SFRAND * IRAND(300));
}


void Defcon::CGhostPart::Move(float fTime)
{
	// All we have to do is move along our predetermined spline 
	// and at the end, rebuild a ghost.

	CEnemy::Move(fTime);

	// We're moving along a spline, so determine our normalized distance along it.

	if(m_fAge < m_fMaxAge)
	{
		m_path.CalcPt(powf(m_fAge / m_fMaxAge, 0.75f), m_pos);
		m_pos.x = gpArena->WrapX(m_pos.x);
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

#if 0
// ------------------------------------------------------------

Defcon::CGhostPrimaryPart::CGhostPrimaryPart()
{
	m_parentType = m_type;
	m_type = ObjType::GHOSTPARTPRIMARY;
}


#ifdef _DEBUG
const char* Defcon::CGhostPrimaryPart::GetClassname() const
{
	static char* psz = "Ghost_primary_part";
	return psz;
};
#endif


void Defcon::CGhostPrimaryPart::SetNewLocation(const CFPoint& pt) 
{
	m_newPos = pt; 
	m_newPos.x = gpArena->WrapX(pt.x);
}


void Defcon::CGhostPrimaryPart::Move(float fTime)
{
	CGhostPart::Move(fTime);

	if(m_fAge >= m_fMaxAge)
	{
		// Our time has wound up, so recreate the ghost that spawned us.
		gpArena->CreateEnemy(ObjType::GHOST, m_newPos, false, false);
	}
}
#endif // 0
