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
	RadarColor(MakeColorFromComponents(255,255,255))
{
	Orientation.Up.Set(0.0f, 1.0f);
	Orientation.Fwd.Set(1.0f, 0.0f);
	BboxRadius.Set(10, 10);
}


Defcon::IGameObject::~IGameObject() 
{
	//UE_LOG(LogGame, Log, TEXT("Game object destructing, type = %s"), *GObjectTypeManager.GetName(GetType()));

	if(IsMissionTarget()) 
	{
		//UE_LOG(LogGame, Log, TEXT("    Game object was a mission target"));
		GDefconGameInstance->HostileDestroyed(GetType());
	}

	if(Sprite)
	{
		// todo: check(!Sprite->HasSlot());
	}
}


void Defcon::IGameObject::CreateSprite(EObjType Kind)
{
	const auto& AtlasInfo = GameObjectResources.Get(Kind);

	Sprite = SNew(Daylon::SpritePlayObject2D);

	Sprite->SetAtlas(AtlasInfo.Atlas->Atlas);
	Sprite->SetSize(AtlasInfo.Size);
	Sprite->UpdateWidgetSize();
}


bool Defcon::IGameObject::ReduceLifespanBy(float f)
{
	Lifespan = FMath::Max(0.0f, Lifespan - f);

	return (Lifespan <= 0.0f);
}


void Defcon::IGameObject::DrawSmall(FPainter& Painter, const I2DCoordMapper& CoordMapper, FSlateBrush& Brush)
{
	CFPoint pt;
	CoordMapper.To(Position, pt);

	const auto S = FVector2D(9, 9);

#if 0
		// Quantize location. -- no, makes screen jitter too much unless we quantize coord mapper
		pt.x = ROUND(pt.x / 9) * 9;
		pt.y = ROUND(pt.y / 9) * 9;
#endif

	const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);

	const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*Painter.OutDrawElements,
		Painter.LayerId,
		Geometry.ToPaintGeometry(),
		&Brush,
		ESlateDrawEffect::None,
		GetRadarColor() * Painter.RenderOpacity);
}


void Defcon::IGameObject::Move(float DeltaTime)
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

	if(Fireblasts())
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
		const int32 frames_per_blast = 12;
		int32 ex = rand() % 3 * frames_per_blast;
		pFireblast->Init(CBitmaps::explo0 + ex,
			frames_per_blast, FRAND * 0.5f + 0.7f,
			fBrightRange, fBrightBase);
		pFireblast->Position = Position;
		const CTrueBitmap& fbb = gBitmaps.GetBitmap(CBitmaps::explo0);
		pFireblast->Position -= CFPoint((float)fbb.GetWidth()/2, -0.5f * fbb.GetHeight());
		pFireblast->Orientation = Orientation;
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

	Position.x = GArena->WrapX(Position.x); //  to be safe

	// If the inertia is some crazy value, zero it.
	if(ABS(Inertia.x) > 2000)
	{
		Inertia.Set(0.0f, 0.0f);
	}

	bMortal = true;
	Lifespan = 0.0f;
	OnAboutToDie();

	//return;

	// todo: implement using particle widget

	// Create an explosion by making
	// several debris objects and 
	// adding them to the debris set.
	int32 n = (int32)(FRAND * 30 + 30);
	float maxsize = FRAND * 5 + 3;

/*
	// Don't use huge particles ever; it 
	// just looks silly in the end.
	if(FRAND < .08)
		maxsize = 14;
*/

	// Define which color to make the debris.
	auto cby = GetExplosionColorBase();
	maxsize *= GetExplosionMass();
	
	if(GetType() != EObjType::HUMAN && IRAND(3) == 1)
	{
		//cby = EColor::Gray;
	}

	bool bDieOff = (FRAND >= 0.25f);
	int32 i;

	float fBrightBase;
	IGameObject* pFireball = CreateFireball(debris, fBrightBase);

	for(i = 0; i < n; i++)
	{
		CFlak* pFlak				= new CFlak;
		pFlak->ColorbaseYoung	= cby;
		pFlak->LargestSize		= maxsize;
		pFlak->bFade				= bDieOff;

		pFlak->Position				= Position;
		pFlak->Orientation				= Orientation;

		CFPoint dir;
		double t = FRAND * TWO_PI;
		
		dir.Set((float)cos(t), (float)sin(t));

		if(bGrounded)
		{
			dir.y = abs(dir.y);
		}

		// Debris has at least the object's momentum.
		pFlak->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		pFlak->Orientation.Fwd *= FRAND * 12.0f + 30.0f;
		// Make the particle have a velocity vector
		// as if it were standing still.
		const float speed = FRAND * 180 + 90;

		pFlak->Orientation.Fwd.MulAdd(dir, speed);

		debris.Add(pFlak);
	}

	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		bDieOff = (FRAND >= 0.25f);
		n = (int32)(FRAND * 20 + 20);
		maxsize = FRAND * 4 + 8.0f;
		//maxsize = FMath::Min(maxsize, 9.0f);

		if(GetType() != EObjType::HUMAN && IRAND(3) == 1)
		{
			cby = EColor::Gray;
		}
		else
		{
			cby = GetExplosionColorBase();
		}

		for(i = 0; i < n; i++)
		{
			CFlak* pFlak = new CFlak;

			pFlak->ColorbaseYoung = cby;
			pFlak->LargestSize = maxsize;
			pFlak->bFade = bDieOff;
			//pFlak->bDrawsOnRadar = false;

			pFlak->Position = Position;
			pFlak->Orientation = Orientation;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.Set((float)cos(t), (float)sin(t));
			if(bGrounded)
				dir.y = abs(dir.y);

			pFlak->Orientation.Fwd = Inertia;

			pFlak->Orientation.Fwd *= FRAND * 12.0f + 30.0f;
			float speed = FRAND * 45 + 22;

			pFlak->Orientation.Fwd.MulAdd(dir, speed);

			debris.Add(pFlak);
		}
	}

#if 0
	if(DEBRIS_SMOKE && pFireball != nullptr)
	{
		const float fLife = FRAND * 10.0f + 3.0f;
		const float fBrightness = fBrightBase;
		//n = (int32)(FRAND * 20 + 30);
		n = (int32)(FRAND * 30 + 70);

		for(i = 0; i < n; i++)
		{
			CPuff* pPuff = new CPuff;
			// Make at least one big buff.
			if(i == n-1)
				pPuff->LargestSize = (float)(maxsize/1.5f * 2 * (FRAND + 1.0f));
			else
				pPuff->LargestSize = (float)(maxsize/1.5f * (FRAND * .5f+ .5f));

			pPuff->MaxAge = pPuff->Lifespan = 
				fLife + SFRAND * 0.125f;
				
			pPuff->m_fBrightest = fBrightness + SFRAND*0.1f;

			pPuff->Position = Position;
			//pPuff->Orientation = Orientation;

			CFPoint dir;
			double t = FRAND * TWO_PI;
			
			dir.Set((float)cos(t), (float)sin(t));

			if(bGrounded)
				dir.y = abs(dir.y);

			// Puff has at least the object's momentum.
			//pPuff->Orientation.Fwd = pObj->Inertia;
			pPuff->Orientation = pFireball->Orientation;

			// Scale the momentum up a bit, otherwise 
			// the explosion looks like it's standing still.
			pPuff->Orientation.Fwd *= FRAND * 2.0f + 3.0f;
			//ndir *= FRAND * 0.4f + 0.2f;
			// Make the particle have a velocity vector
			// as if it were standing still.
			float speed = FRAND * 9 + 4;

			CFPoint speed_((FRAND*2+2)*speed, speed);

			pPuff->Orientation.Fwd.MulAdd(dir, speed_);


			debris.Add(pPuff);
		}
	}
#endif
}


Defcon::IGameObject* Defcon::IGameObject::CreateFireball(CGameObjectCollection& debris, float& fBrightBase)
{
	// Create fireball.

	if(Fireballs())
	{
		auto pFireball = new CBitmapDisplayer;
		pFireball->SetType(EObjType::EXPLOSION);
		pFireball->Position = Position;
		pFireball->Orientation = Orientation;

		const auto& Info = GameObjectResources.Get(EObjType::EXPLOSION);
		pFireball->Lifespan =  1.0f / Info.Atlas->Atlas.FrameRate * Info.Atlas->Atlas.NumCels;

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

	if(Fireballs())
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

		const int32 frames_per_blast = 12;
		int32 ex = rand() % 3 * frames_per_blast;
		pFireball->Init(CBitmaps::explo0 + ex,
			frames_per_blast, FRAND * 0.5f + 0.7f, fBrightRange, fBrightBase);
		pFireball->Position = Position;
		const CTrueBitmap& fbb = gBitmaps.GetBitmap(CBitmaps::explo0);
		pFireball->Position -= CFPoint(fbb.GetWidth() / 2.0f, -0.5f * fbb.GetHeight());
		pFireball->Orientation = Orientation;
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
		//UE_LOG(LogGame, Log, TEXT("Installing sprite for object class %s"), *GObjectTypeManager.GetName(Type));
		Daylon::Install<SDaylonSprite>(Sprite, 0.5f); 
	} 
}


void Defcon::IGameObject::UninstallSprite() 
{
	if(Sprite) 
	{
		//UE_LOG(LogGame, Log, TEXT("Uninstalling sprite for object class %s"), *GObjectTypeManager.GetName(Type));
		Daylon::Uninstall(Sprite); 
	} 
}


void                 Defcon::IGameObject::Init(const CFPoint& InArenaSize, const CFPoint& InScreenSize) { ArenaSize = InArenaSize; ScreenSize = InScreenSize; }
void                 Defcon::IGameObject::Draw(FPainter& Painter, const I2DCoordMapper& mapper) {}

void                 Defcon::IGameObject::OnFinishedCreating()                 {}
void                 Defcon::IGameObject::OnAboutToDie()                       {}
void                 Defcon::IGameObject::Notify(EMessage, void*)               {}
void                 Defcon::IGameObject::GetInjurePt(CFPoint&) const          {}
bool                 Defcon::IGameObject::TestInjury(const CFRect&) const      { return false; }
Defcon::IGameObject* Defcon::IGameObject::GetNext()                            { return NextPtr; }
Defcon::IGameObject* Defcon::IGameObject::GetPrev()                            { return PrevPtr; }
void                 Defcon::IGameObject::SetNext(IGameObject* p)              { NextPtr = p; }
void                 Defcon::IGameObject::SetPrev(IGameObject* p)              { PrevPtr = p; }
Defcon::EObjType     Defcon::IGameObject::GetParentType() const                { return ParentType; }
Defcon::EObjType     Defcon::IGameObject::GetCreatorType() const               { return CreatorType; }
void                 Defcon::IGameObject::SetCreatorType(EObjType n)           { CreatorType = n; }
Defcon::EObjType     Defcon::IGameObject::GetType() const                      { return Type; }
void                 Defcon::IGameObject::SetType(EObjType n)                  { Type = n; }
bool                 Defcon::IGameObject::OccursFrequently() const             { return false; }
bool                 Defcon::IGameObject::Fireballs() const                    { return false; }
float                Defcon::IGameObject::GetExplosionMass() const             { return 1.0f; }
bool                 Defcon::IGameObject::Fireblasts() const                   { return false; }
void                 Defcon::IGameObject::ZeroVelocity()                       { Velocity.Set(0,0); }
const CFPoint&       Defcon::IGameObject::GetVelocity() const                  { return Velocity; }
void                 Defcon::IGameObject::MakeHurtable(bool b)                 { bCanBeInjured = b; }
bool                 Defcon::IGameObject::IsMortal() const                     { return bMortal; }
void                 Defcon::IGameObject::MarkAsDead()                         { Lifespan = 0.0001f; bMortal = true; }
bool                 Defcon::IGameObject::MarkedForDeath() const               { return (bMortal && Lifespan <= 0.0001f); }
float                Defcon::IGameObject::GetCollisionForce() const            { return 0.01f * COLLISION_DAMAGE; } // todo: s/b a per-class value and multiply by the object's velocity.
bool                 Defcon::IGameObject::IsCollisionInjurious() const         { return bIsCollisionInjurious; }
void                 Defcon::IGameObject::SetCollisionInjurious(bool b)        { bIsCollisionInjurious = b; }
bool                 Defcon::IGameObject::IsInjurious() const                  { return bInjurious; }
bool                 Defcon::IGameObject::CanBeInjured() const                 { return bCanBeInjured; }
int32                Defcon::IGameObject::GetPointValue() const                { return PointValue; }
bool                 Defcon::IGameObject::ExternallyOwned() const              { return bExternallyOwned; }
void                 Defcon::IGameObject::SetExternalOwnership(bool b)         { bExternallyOwned = b; }
bool                 Defcon::IGameObject::IsMissionTarget() const              { return bMissionTarget; }
void                 Defcon::IGameObject::SetAsMissionTarget(bool b)           { bMissionTarget = b; }
const FLinearColor&  Defcon::IGameObject::GetRadarColor() const                { return RadarColor; }
Defcon::EColor       Defcon::IGameObject::GetExplosionColorBase() const        { return EColor::Gray; }
bool                 Defcon::IGameObject::IsOurPositionVisible() const         { return GArena->IsPointVisible(Position); }



#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
