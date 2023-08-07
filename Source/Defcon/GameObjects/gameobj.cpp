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


void Defcon::IGameObject::AddExplosionDebris(const FExplosionParams& Params, CGameObjectCollection& Debris)
{
	for(int32 i = 0; i < Params.NumParticles; i++)
	{
		auto Particle = new CFlak;
		
		Particle->ColorbaseYoung = Params.YoungColor[BRAND ? 0 : 1];
		Particle->ColorbaseOld   = Params.OldColor[BRAND ? 0 : 1];
		Particle->bCold          = Params.bCold;
		Particle->LargestSize    = Params.MaxParticleSize;
		Particle->bFade          = Params.bFade;

		Particle->Position    = Position;
		Particle->Orientation = Orientation;

		CFPoint dir;
		dir.SetRandomVector();

		if(bGrounded)
		{
			dir.y = ABS(dir.y);
		}

		// Debris has at least the object's momentum.
		Particle->Orientation.Fwd = Inertia;

		// Scale the momentum up a bit, otherwise 
		// the explosion looks like it's standing still.
		Particle->Orientation.Fwd *= FRANDRANGE(20, 32) * Params.InertiaScale;
		
		//Particle->Orientation.Fwd.MulAdd(dir, FRANDRANGE(110, 140));
		Particle->Orientation.Fwd.MulAdd(dir, FRANDRANGE(Params.MinSpeed, Params.MaxSpeed));

		Debris.Add(Particle);
	}
}


void Defcon::IGameObject::Explode(CGameObjectCollection& Debris)
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

	auto cby = GetExplosionColorBase();
	
	float fBrightBase;
	IGameObject* pFireball = CreateExplosionFireball(Debris, fBrightBase);


	FExplosionParams Params;

	Params.NumParticles    = (int32)FRANDRANGE(30, 60);
	Params.MaxParticleSize = FRANDRANGE(3, 8) * GetExplosionMass();
	Params.MinSpeed        =  90.0f;
	Params.MaxSpeed        = 270.0f;
	Params.YoungColor[0]   = 
	Params.YoungColor[1]   = cby;
	Params.bFade           = (FRAND >= 0.25f);

	AddExplosionDebris(Params, Debris);


	if(FRAND <= DEBRIS_DUAL_PROB)
	{
		Params.NumParticles    = (int32)FRANDRANGE(20, 40);
		Params.MaxParticleSize = FRANDRANGE(8.0f, 12.0f);
		Params.YoungColor[0]   = 
		Params.YoungColor[1]   = (GetType() != EObjType::HUMAN && IRAND(3) == 1) ? EColor::Gray : GetExplosionColorBase();
		Params.MinSpeed        =  22.0f;
		Params.MaxSpeed        =  67.0f;
		Params.bFade           = (FRAND >= 0.25f);

		AddExplosionDebris(Params, Debris);
	}
}


Defcon::IGameObject* Defcon::IGameObject::CreateExplosionFireball(CGameObjectCollection& Debris, float& /*fBrightBase*/)
{
	// Create fireball.

	if(ExplosionHasFireball())
	{
		auto ExplosionFireball = new CBitmapDisplayer;
		ExplosionFireball->SetType(EObjType::EXPLOSION);
		ExplosionFireball->Position = Position;
		ExplosionFireball->Orientation = Orientation;

		const auto& Info = GameObjectResources.Get(EObjType::EXPLOSION);
		ExplosionFireball->Lifespan =  1.0f / Info.Atlas->Atlas.FrameRate * Info.Atlas->Atlas.NumCels;

		ExplosionFireball->Sprite = Daylon::SpawnSpritePlayObject2D(Info.Atlas->Atlas, Info.Size, Info.Radius);
		ExplosionFireball->Sprite->FlipHorizontal = Daylon::RandBool();
		ExplosionFireball->Sprite->FlipVertical   = Daylon::RandBool();

		Debris.Add(ExplosionFireball);
		return ExplosionFireball;
	}

	return nullptr; 
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
void                 Defcon::IGameObject::Notify(EMessage, void*)              {}
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
bool                 Defcon::IGameObject::ExplosionHasFireball() const         { return false; }
float                Defcon::IGameObject::GetExplosionMass() const             { return 1.0f; }
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
bool                 Defcon::IGameObject::IsMissionTarget() const              { return bMissionTarget; }
void                 Defcon::IGameObject::SetAsMissionTarget(bool b)           { bMissionTarget = b; }
const FLinearColor&  Defcon::IGameObject::GetRadarColor() const                { return RadarColor; }
Defcon::EColor       Defcon::IGameObject::GetExplosionColorBase() const        { return EColor::Gray; }
bool                 Defcon::IGameObject::IsOurPositionVisible() const         { return GArena->IsPointVisible(Position); }
//bool                 Defcon::IGameObject::ExternallyOwned() const              { return bExternallyOwned; }
//void                 Defcon::IGameObject::SetExternalOwnership(bool b)         { bExternallyOwned = b; }



#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
