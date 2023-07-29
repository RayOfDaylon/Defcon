// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// player.cpp


#include "player.h"

#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"

#include "Common/util_core.h"
#include "Common/util_str.h"
#include "Globals/GameColors.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Common/gamma.h"
#include "GameObjects/Enemies/enemies.h"
#include "flak.h"
#include "bmpdisp.h"
#include "human.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconLogging.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




Defcon::CPlayer::CPlayer()
{
	m_type = ObjType::PLAYER;

	// Start off invincible, switch after birth finished.
	m_bMortal = false;
	m_bCanBeInjured = false;//true;
	//m_bBirthsoundPlayed = false;

	m_laserWeapon.MountOnto(*this, CFPoint(0,0));
	m_laserWeapon.SetEmissionPt(CFPoint(0, -8)); // todo: could improve this

	this->SetShieldStrength(1.0f);

	CreateSprite(m_type);

	const auto& Info = GameObjectResources.Get(m_type);
	m_bboxrad.set(Info.Size.X * 0.25f, Info.Size.Y * 0.25f);

	// Make our "pickup human" bboxrad more generous than hitbox.
	m_bboxradPickup.set(Info.Size.X * 0.4f, Info.Size.Y * 0.4f);
}


void Defcon::CPlayer::InitPlayer(float fw)
{
	m_fDrag = PLAYER_DRAG;//0.1f;
	m_maxThrust = PLAYER_MAXTHRUST;
	m_fMass = PLAYER_MASS;

	m_laserWeapon.m_fArenawidth = fw;

	// Arrange the birth debris in a circle.
	// Don't do this in ctor since we don't 
	// know where player is yet.

#if 0
	const int n = array_size(m_birthDebrisLocs);

	for(int i = 0; i < n; i++)
	{
		m_debrisPow[i] = FRAND * 8 + 0.1f;
		float u = (float)i / n;
		u *= (float)TWO_PI;

		float fRad = FRAND * PLAYER_BIRTHDEBRISDIST;
		
		m_birthDebrisLocsOrg[i].set((float)cos(u) * fRad, (float)sin(u) * fRad * .33f);
		m_birthDebrisLocsOrg[i] += m_pos;

		if(m_birthDebrisLocsOrg[i].x < 0)
		{
			m_birthDebrisLocsOrg[i].x += fw;
		}
		else if(m_birthDebrisLocsOrg[i].x >= fw)
		{
			m_birthDebrisLocsOrg[i].x -= fw;
		}
	}
#endif
}


Defcon::CPlayer::~CPlayer() {}

#ifdef _DEBUG
const char* Defcon::CPlayer::GetClassname() const
{
	static char* psz = "Player";
	return psz;
}
#endif


void Defcon::CPlayer::SetIsAlive(bool b)
{
	ILiveGameObject::SetIsAlive(b);

	EnableInput(b);

	// Play arena activate/deactivate take care of installing/uninstalling sprite
	// required by arena transition.

	if(Sprite)
	{
		Sprite->Show(b);
	}

	if(b)
	{
		SetShieldStrength(1.0f);
	}
	else
	{
	}
}


void Defcon::CPlayer::OnAboutToDie()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	CGameObjectCollection& Humans = gpArena->GetHumans();

	Humans.ForEach([this](IGameObject* pObj)
	{
		CHuman& Human = (CHuman&)*pObj;
		if(Human.GetCarrier() == this)
		{
			pObj->Notify(Message::carrier_killed, this);
		}
	});
}


bool Defcon::CPlayer::EmbarkPassenger(IGameObject* pObj, CGameObjectCollection& humans)
{
	if(this->MarkedForDeath())
	{
		return false;
	}

	pObj->Notify(Message::takenaboard, this);
#ifdef _DEBUG
	char sz[100];
	MySprintf(sz, "Human picked up by player.\n");
	OutputDebugString(sz);
#endif
	return true;
}




bool Defcon::CPlayer::DebarkOnePassenger(CGameObjectCollection& humans)
{
	IGameObject* pObj = humans.GetFirst();

	while(pObj != nullptr)
	{
		CHuman& human = (CHuman&)*pObj;
		if(human.GetCarrier() == this)
		{
#ifdef _DEBUG
			char sz[100];
			MySprintf(sz, "Human released by player\n");
			OutputDebugString(sz);
#endif
			pObj->Notify(Message::released, this);
			return true;
		}
		pObj = pObj->GetNext();
	}

	return false;
}


bool Defcon::CPlayer::IsSolid() const 
{
	return (m_fAge > PLAYER_BIRTHDURATION);
}


void Defcon::CPlayer::Move(float fElapsedTime)
{
#if 0
	// test mapper.
	{
		CFPoint in, out, final;
		for(in.x = -50; in.x < 50; in.x++)
		{
			m_pMapper->To(in, out);
			m_pMapper->From(out, final);
			int inx = ROUND(in.x);
			int fx = ROUND(final.x);
			if(inx != fx)
			{
				char sz[100];
				MySprintf(sz, "Mapping x = %.3f to %.3f back to %.3f\n", in.x, out.x, final.x);
				OutputDebugString(sz);
			}
		}
	}
#endif
	ILiveGameObject::Move(fElapsedTime);

#if 0
	if(!m_bBirthsoundPlayed && m_fAge > PLAYER_BIRTHDURATION * .66f)
	{
		gpAudio->OutputSound(snd_ship_materialize);
		m_bBirthsoundPlayed = true;
	}
#endif

#if 0
	// todo: visualize materialization
	if(m_fAge < PLAYER_BIRTHDURATION)
	{
		// Bring in birth debris.
		float t = m_fAge / PLAYER_BIRTHDURATION;
		int n = array_size(m_birthDebrisLocs);

		CFPoint pt;

		const float fw = m_laserWeapon.m_fArenawidth;

		for(int i = 0; i < n; i++)
		{
			m_birthDebrisLocsOrg[i] += m_inertia * (FRAND * 0.9f + 0.1f);

			if(m_birthDebrisLocsOrg[i].x < 0)
				m_birthDebrisLocsOrg[i].x += fw;

			if(m_birthDebrisLocsOrg[i].x >= fw)
				m_birthDebrisLocsOrg[i].x -= fw;

			float ft = t;
#if 1
			gpArena->Lerp(
				m_birthDebrisLocsOrg[i],  
				m_pos, 
				m_birthDebrisLocs[i],
				(float)pow(ft, m_debrisPow[i]));
#else
			m_pMapper->To(m_birthDebrisLocsOrg[i], pt);
			pt.lerp(shipLoc, (float)pow(ft, m_debrisPow[i]));
			m_pMapper->From(pt, m_birthDebrisLocs[i]);
#endif
		}
	}
	else
#endif
	{
		m_bCanBeInjured = true;
	}

	float fs = this->GetShieldStrength();

	if(fs < 1.0f)
	{
		fs = FMath::Min(1.0f, fs + fElapsedTime / 20);
		this->SetShieldStrength(fs);
	}

	Sprite->FlipHorizontal = (m_orient.fwd.x < 0);
}


void Defcon::CPlayer::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& mapper, FSlateBrush&)
{
	// Draw a white 5 x 5 px diamond.
	CFPoint pt;

	mapper.To(m_pos, pt);

	const auto S = FVector2D(12, 12);// RadarBrush.GetImageSize();

	const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);

	const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*framebuf.OutDrawElements,
		framebuf.LayerId,
		Geometry.ToPaintGeometry(),
		&RadarBrush, // todo: can use provided brush
		ESlateDrawEffect::None,
		RadarBrush.TintColor.GetSpecifiedColor() * framebuf.RenderOpacity);
}


void Defcon::CPlayer::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


void Defcon::CPlayer::FireLaserWeapon(CGameObjectCollection& goc)
{
	m_laserWeapon.Fire(goc);
	if(m_velocity.y != 0 && FRAND <= LASER_MULTI_PROB)
	{
		// Fire extra bolts in the vthrust dir.
		for(int32 i = 0; i < LASER_EXTRA_COUNT; i++)
		{
			CFPoint off(0.0f, m_velocity.y > 0 ? 1.0f : -1.0f);
			if(FRAND > 0.5f)
				off *= 2;
			CFPoint backup(m_pos);
			m_pos += off;
			m_laserWeapon.Fire(goc);
			m_pos = backup;
		}
	}
#if 0
	m_laserWeapon.Fire(goc);
	if(m_velocity.y != 0 && FRAND > 0.75f)
	{
		// Fire another bolt 2 pixels in the vthrust dir.
		CFPoint off(0.0f, m_velocity.y > 0 ? 1.0f : -1.0f);
		if(FRAND > 0.5f)
			off *= 2;
		CFPoint backup(m_pos);
		m_pos += off;
		m_laserWeapon.Fire(goc);
		m_pos = backup;
	}
#endif
}


void Defcon::CPlayer::ImpartForces(float frameTime)
{
	if(!CONTROLLER_EXPLICIT_REVERSE)
	{
		Super::ImpartForces(frameTime);
		return;
	}

	if(!m_bCanMove)
	{
		return;
	}

	// Let parent class handle horizontal forces.
	m_thrustVector.y = 0.0f;
	Super::ImpartForces(frameTime);

	// Now handle vertical force in our way.
	const float kVertMotionPxPerSec = 300.0f;

	if(m_navCtls[ctlUp].bActive)
	{
		const float timeHeld = GameTime() - m_navCtls[ctlUp].fTimeDown;
		m_pos.muladd({ 0.0f, FMath::Min((kVertMotionPxPerSec * timeHeld * 2) + kVertMotionPxPerSec/2, kVertMotionPxPerSec) }, frameTime);
	}
	else if(m_navCtls[ctlDown].bActive)
	{
		const float timeHeld = GameTime() - m_navCtls[ctlDown].fTimeDown;
		m_pos.muladd({ 0.0f, -FMath::Min((kVertMotionPxPerSec * timeHeld * 2) + kVertMotionPxPerSec/2, kVertMotionPxPerSec) }, frameTime);
	}
}


void Defcon::CPlayer::Explode(CGameObjectCollection& debris)
{
	if(this->MarkedForDeath())
	{
		return;
	}

	gpAudio->OutputSound(Defcon::snd_player_dying);
	//time_wait(250);
 

	// Mark it to die immediately.
	// We use to make it die in 1/10th second, 
	// because that produced a cool "dying in progress" effect
	// where the object would be exploding and still visible
	// for a brief while. But that causes a lot of problems 
	// too with object relationships.

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	// Create an explosion by making
	// several debris objects and 
	// adding them to the debris set.
	int n = (int)(10 * (FRAND * 30 + 30));
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
	
	if(this->GetType() != ObjType::HUMAN && IRAND(3) == 1)
		cby = CGameColors::gray;

	bool bDieOff = (FRAND >= 0.25f);
	int i;
#if 0
	// Create fireball.
	if(this->Fireballs())
	{
		float fBrightRange, fBrightBase;
		CBitmapDisplayer* pFireball = nullptr;

		pFireball = new CBitmapDisplayer;
		if(BRAND)
		{
			fBrightRange= FRAND * 0.75f;
			fBrightBase = FRAND * 0.1f + 0.15f;
		}
		else
		{
			fBrightRange= FRAND * 0.25f;
			fBrightBase = FRAND * 0.1f + 0.65f;
		}
		const int32 frames_per_blast = 12;
		int32 ex = rand() % 4 * frames_per_blast;
		pFireball->Init(CBitmaps::explo0 + ex,
			frames_per_blast, FRAND * 0.5f + 0.7f,
			fBrightRange, fBrightBase);
		pFireball->m_pos = m_pos;
		const CTrueBitmap& fbb = gBitmaps.GetBitmap(CBitmaps::explo0);
		pFireball->m_pos -= CFPoint(
			(float)fbb.GetWidth()/2, -0.5f * fbb.GetHeight());
		pFireball->m_orient = m_orient;
		debris.Add(pFireball);
	}
#endif


	for(i = 0; i < n; i++)
	{
		CFlak* pFlak = new CFlak;
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
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
