// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	gameobj.cpp
	Base game entity class
*/


#include "gameobj.h"

#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/flak.h"
#include "Main/mapper.h"
#include "Arenas/DefconPlayViewBase.h"

#include "DefconUtils.h"
#include "DefconLogging.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Defcon::IGameObject::IGameObject()
	:
	m_smallColor(MakeColorFromComponents(255,255,255))
{
	m_orient.up.set(0.0f, 1.0f);
	m_orient.fwd.set(1.0f, 0.0f);
	m_bboxrad.set(10, 10);
}


Defcon::IGameObject::~IGameObject() 
{
	if(this->IsMissionTarget()) 
	{
		gDefconGameInstance->HostileDestroyed(GetType());
	}

	if(Sprite)
	{
		// todo: check(!Sprite->HasSlot());
	}
}


void Defcon::IGameObject::CreateSprite(ObjType Kind)
{
	const auto& AtlasInfo = GameObjectResources.Get(Kind);

	Sprite = SNew(Daylon::SpritePlayObject2D);

	Sprite->SetAtlas(AtlasInfo.Atlas->Atlas);
	Sprite->SetSize(AtlasInfo.Size);
	Sprite->UpdateWidgetSize();
}


bool Defcon::IGameObject::ReduceLifespanBy(float f)
{
	m_fLifespan = FMath::Max(0.0f, m_fLifespan - f);

	return (m_fLifespan <= 0.0f);
}


void Defcon::IGameObject::DrawSmall(FPaintArguments& FrameBuffer, const I2DCoordMapper& CoordMapper, FSlateBrush& Brush)
{
	CFPoint pt;
	CoordMapper.To(m_pos, pt);

	const auto S = FVector2D(9, 9);

#if 0
		// Quantize location. -- no, makes screen jitter too much unless we quantize coord mapper
		pt.x = ROUND(pt.x / 9) * 9;
		pt.y = ROUND(pt.y / 9) * 9;
#endif

	const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);

	const auto Geometry = FrameBuffer.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*FrameBuffer.OutDrawElements,
		FrameBuffer.LayerId,
		Geometry.ToPaintGeometry(),
		&Brush,
		ESlateDrawEffect::None,
		GetRadarColor() * FrameBuffer.RenderOpacity * FrameBuffer.InWidgetStyle->GetColorAndOpacityTint().A);
}


void Defcon::IGameObject::Move(float fElapsedTime)
{
	// Default motion behavior for an object.
}


Defcon::IGameObject* Defcon::IGameObject::CreateFireblast(CGameObjectCollection& debris, float& fBrightBase)
{
	// Create fireball.
#if 0
	// todo: maybe we want to implement this with UE sprite? Some stuff like ghost and pod use it, I think.

	float fBrightRange;
	CBitmapDisplayer* pFireblast = nullptr;

	if(this->Fireblasts())
	{
		pFireblast = new CBitmapDisplayer;
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
		const size_t frames_per_blast = 12;
		size_t ex = rand() % 3 * frames_per_blast;
		pFireblast->Init(CBitmaps::explo0 + ex,
			frames_per_blast, FRAND * 0.5f + 0.7f,
			fBrightRange, fBrightBase);
		pFireblast->m_pos = m_pos;
		const CTrueBitmap& fbb = gBitmaps.GetBitmap(CBitmaps::explo0);
		pFireblast->m_pos -= CFPoint((float)fbb.GetWidth()/2, -0.5f * fbb.GetHeight());
		pFireblast->m_orient = m_orient;
		debris.Add(pFireblast);
		return pFireblast;
	}
#endif
	return nullptr;
}


void Defcon::IGameObject::Explode(CGameObjectCollection& debris)
{
	// Default way an object explodes.

	check(this != nullptr);


	// Mark it to die immediately.
	// We use to make it die in 1/10th second, 
	// because that produced a cool "dying in progress" effect
	// where the object would be exploding and still visible
	// for a brief while. But that causes a lot of problems 
	// too with object relationships.

	m_pos.x = gpArena->WrapX(m_pos.x); //  to be safe

	// If the inertia is some crazy value, zero it.
	if(ABS(m_inertia.x) > 2000)
	{
		m_inertia.set(0.0f, 0.0f);
	}

	m_bMortal = true;
	m_fLifespan = 0.0f;
	this->OnAboutToDie();

	//return;

	// todo: implement using particle widget

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
	
	if(this->GetType() != ObjType::HUMAN && IRAND(3) == 1)
	{
		//cby = CGameColors::gray;
	}

	bool bDieOff = (FRAND >= 0.25f);
	int i;

	float fBrightBase;
	IGameObject* pFireball = this->CreateFireball(debris, fBrightBase);

	for(i = 0; i < n; i++)
	{
		CFlak* pFlak				= new CFlak;
		pFlak->m_eColorbaseYoung	= cby;
		pFlak->m_fLargestSize		= maxsize;
		pFlak->m_bFade				= bDieOff;

		pFlak->m_pos				= m_pos;
		pFlak->m_orient				= m_orient;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.set((float)cos(t), (float)sin(t));

		if(m_grounded)
		{
			dir.y = abs(dir.y);
		}

		// Debris has at least the object's momentum.
		pFlak->m_orient.fwd = m_inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
		// Make the particle have a velocity vector
		// as if it were standing still.
		const float speed = FRAND * 180 + 90;

		pFlak->m_orient.fwd.muladd(dir, speed);

		debris.Add(pFlak);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		n = (int)(FRAND * 20 + 20);
		maxsize = FRAND * 4 + 8.0f;
		//maxsize = FMath::Min(maxsize, 9.0f);

		if(this->GetType() != ObjType::HUMAN && IRAND(3) == 1)
		{
			cby = CGameColors::gray;
		}
		else
		{
			cby = this->GetExplosionColorBase();
		}

		for(i = 0; i < n; i++)
		{
			CFlak* pFlak = new CFlak;

			pFlak->m_eColorbaseYoung = cby;
			pFlak->m_fLargestSize = maxsize;
			pFlak->m_bFade = bDieOff;
			pFlak->m_bDrawSmall = false;

			pFlak->m_pos = m_pos;
			pFlak->m_orient = m_orient;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.set((float)cos(t), (float)sin(t));
			if(m_grounded)
				dir.y = abs(dir.y);

			pFlak->m_orient.fwd = m_inertia;

			pFlak->m_orient.fwd *= FRAND * 12.0f + 30.0f;
			float speed = FRAND * 45 + 22;

			pFlak->m_orient.fwd.muladd(dir, speed);

			debris.Add(pFlak);
		}
	}

#if 0
	if(DEBRIS_SMOKE && pFireball != nullptr)
	{
		const float fLife = FRAND * 10.0f + 3.0f;
		const float fBrightness = fBrightBase;
		//n = (int)(FRAND * 20 + 30);
		n = (int)(FRAND * 30 + 70);

		for(i = 0; i < n; i++)
		{
			CPuff* pPuff = new CPuff;
			// Make at least one big buff.
			if(i == n-1)
				pPuff->m_fLargestSize = (float)(maxsize/1.5f * 2 * (FRAND + 1.0f));
			else
				pPuff->m_fLargestSize = (float)(maxsize/1.5f * (FRAND * .5f+ .5f));

			pPuff->m_maxAge = pPuff->m_fLifespan = 
				fLife + SFRAND * 0.125f;
				
			pPuff->m_fBrightest = fBrightness + SFRAND*0.1f;

			pPuff->m_pos = m_pos;
			//pPuff->m_orient = m_orient;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.set((float)cos(t), (float)sin(t));

			if(m_grounded)
				dir.y = abs(dir.y);

			// Puff has at least the object's momentum.
			//pPuff->m_orient.fwd = pObj->m_inertia;
			pPuff->m_orient = pFireball->m_orient;

			// Scale the momentum up a bit, otherwise 
			// the explosion looks like it's standing still.
			pPuff->m_orient.fwd *= FRAND * 2.0f + 3.0f;
			//ndir *= FRAND * 0.4f + 0.2f;
			// Make the particle have a velocity vector
			// as if it were standing still.
			float speed = FRAND * 9 + 4;

			CFPoint speed_((FRAND*2+2)*speed, speed);

			pPuff->m_orient.fwd.muladd(dir, speed_);


			debris.Add(pPuff);
		}
	}
#endif
}


Defcon::IGameObject* Defcon::IGameObject::CreateFireball(CGameObjectCollection& debris, float& fBrightBase)
{
	// Create fireball.

	if(this->Fireballs())
	{
		auto pFireball = new CBitmapDisplayer;
		pFireball->SetType(ObjType::EXPLOSION);
		pFireball->m_pos = m_pos;
		pFireball->m_orient = m_orient;

		const auto& Info = GameObjectResources.Get(ObjType::EXPLOSION);
		pFireball->m_fLifespan =  1.0f / Info.Atlas->Atlas.FrameRate * Info.Atlas->Atlas.NumCels;

		pFireball->Sprite = Daylon::SpawnSpritePlayObject2D(Info.Atlas->Atlas, Info.Size, Info.Radius);
		pFireball->Sprite->FlipHorizontal = Daylon::RandBool();
		pFireball->Sprite->FlipVertical   = Daylon::RandBool();

		debris.Add(pFireball);
		return pFireball;
	}

	return nullptr; 

#if 0
	float fBrightRange;
	CBitmapDisplayer* pFireball = nullptr;

	if(this->Fireballs())
	{
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

		const size_t frames_per_blast = 12;
		size_t ex = rand() % 3 * frames_per_blast;
		pFireball->Init(CBitmaps::explo0 + ex,
			frames_per_blast, FRAND * 0.5f + 0.7f, fBrightRange, fBrightBase);
		pFireball->m_pos = m_pos;
		const CTrueBitmap& fbb = gBitmaps.GetBitmap(CBitmaps::explo0);
		pFireball->m_pos -= CFPoint(fbb.GetWidth() / 2.0f, -0.5f * fbb.GetHeight());
		pFireball->m_orient = m_orient;
		debris.Add(pFireball);
		return pFireball;
	}
	return nullptr;
#endif
}


void Defcon::IGameObject::InstallSprite() 
{
	if(Sprite) 
	{
		//UE_LOG(LogGame, Log, TEXT("Installing sprite for object class %s"), *ObjectTypeManager.GetName(m_type));
		Daylon::Install<SDaylonSprite>(Sprite, 0.5f); 
	} 
}


void Defcon::IGameObject::UninstallSprite() 
{
	if(Sprite) 
	{
		//UE_LOG(LogGame, Log, TEXT("Uninstalling sprite for object class %s"), *ObjectTypeManager.GetName(m_type));
		Daylon::Uninstall(Sprite); 
	} 
}


void                 Defcon::IGameObject::Init(const CFPoint& ArenaSize, const CFPoint& ScreenSize)     { m_arenasize = ArenaSize; m_screensize = ScreenSize; }
void                 Defcon::IGameObject::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper) {}

void                 Defcon::IGameObject::OnFinishedCreating()                 {}
void                 Defcon::IGameObject::OnAboutToDie()                       {}
void                 Defcon::IGameObject::Notify(Message, void*)               {}
void                 Defcon::IGameObject::GetInjurePt(CFPoint&) const          {}
bool                 Defcon::IGameObject::TestInjury(const CFRect&) const      { return false; }
Defcon::IGameObject* Defcon::IGameObject::GetNext()                            { return m_pNext; }
Defcon::IGameObject* Defcon::IGameObject::GetPrev()                            { return m_pPrev; }
void                 Defcon::IGameObject::SetNext(IGameObject* p)              { m_pNext = p; }
void                 Defcon::IGameObject::SetPrev(IGameObject* p)              { m_pPrev = p; }
Defcon::ObjType      Defcon::IGameObject::GetParentType() const                { return m_parentType; }
Defcon::ObjType      Defcon::IGameObject::GetCreatorType() const               { return m_creatorType; }
void                 Defcon::IGameObject::SetCreatorType(ObjType n)            { m_creatorType = n; }
Defcon::ObjType      Defcon::IGameObject::GetType() const                      { return m_type; }
void                 Defcon::IGameObject::SetType(ObjType n)                   { m_type = n; }
bool                 Defcon::IGameObject::OccursFrequently() const             { return false; }
bool                 Defcon::IGameObject::Fireballs() const                    { return false; }
float                Defcon::IGameObject::GetExplosionMass() const             { return 1.0f; }
bool                 Defcon::IGameObject::Fireblasts() const                   { return false; }
void                 Defcon::IGameObject::ZeroVelocity()                       { m_velocity.set(0,0); }
const CFPoint&       Defcon::IGameObject::GetVelocity() const                  { return m_velocity; }
void                 Defcon::IGameObject::MakeHurtable(bool b)                 { m_bCanBeInjured = b; }
bool                 Defcon::IGameObject::IsMortal() const                     { return m_bMortal; }
void                 Defcon::IGameObject::MarkAsDead()                         { m_fLifespan = 0.0001f; m_bMortal = true; }
bool                 Defcon::IGameObject::MarkedForDeath() const               { return (m_bMortal && m_fLifespan <= 0.0001f); }
float                Defcon::IGameObject::GetCollisionForce() const            { return 0.01f * COLLISION_DAMAGE; } // todo: s/b a per-class value and multiply by the object's velocity.
bool                 Defcon::IGameObject::IsCollisionInjurious() const         { return m_bIsCollisionInjurious; }
void                 Defcon::IGameObject::SetCollisionInjurious(bool b)        { m_bIsCollisionInjurious = b; }
bool                 Defcon::IGameObject::IsInjurious() const                  { return m_bInjurious; }
bool                 Defcon::IGameObject::CanBeInjured() const                 { return m_bCanBeInjured; }
size_t               Defcon::IGameObject::GetPointValue() const                { return m_pointValue; }
bool                 Defcon::IGameObject::ExternallyOwned() const              { return m_bExternallyOwned; }
void                 Defcon::IGameObject::SetExternalOwnership(bool b)         { m_bExternallyOwned = b; }
bool                 Defcon::IGameObject::IsMissionTarget() const              { return m_bMissionTarget; }
void                 Defcon::IGameObject::SetAsMissionTarget(bool b)           { m_bMissionTarget = b; }
const FLinearColor&  Defcon::IGameObject::GetRadarColor() const                { return m_smallColor; }
int                  Defcon::IGameObject::GetExplosionColorBase() const        { return CGameColors::gray; }



#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
