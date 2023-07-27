/*
	gameobj.cpp
	Base game entity class
	Copyright 2003-2004 Daylon Graphics Ltd.
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


#pragma optimize("", off)

Defcon::IGameObject::IGameObject()
	:
	m_pMapper(nullptr),
	m_fLifespan(1.0f),
	m_bDrawSmall(true),
	m_bMortal(false),
	m_bExternallyOwned(false),
	m_fAge(0.0f),
	m_fMass(1.0f),
	m_fDrag(0.0f),
	m_pPrev(nullptr),
	m_pNext(nullptr),
	m_bInjurious(false),
	m_bCanBeInjured(false),
	m_bIsCollisionInjurious(false),
	m_parentType(ObjType::UNKNOWN),
	m_type(ObjType::UNKNOWN),
	m_creatorType(ObjType::UNKNOWN),
	m_smallColor(MakeColorFromComponents(255,255,255)),
	m_pointValue(0),
	m_fAnimSpeed(1.0f),
	m_bMissionTarget(false)
{
	m_orient.up.set(0.0f, 1.0f);
	m_orient.fwd.set(1.0f, 0.0f);
	//m_fNow = UKismetSystemLibrary::GetGameTimeInSeconds(gpArena);
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


void Defcon::IGameObject::OnAboutToDie()
{
}


void Defcon::IGameObject::Notify(Message, void*)
{
}


void Defcon::IGameObject::GetInjurePt(CFPoint&) const
{
}


bool Defcon::IGameObject::TestInjury(const CFRect&) const
{
	return false;
}


void Defcon::IGameObject::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
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


void Defcon::IGameObject::InstallSprite() 
{
	if(Sprite) 
	{
		UE_LOG(LogGame, Log, TEXT("Installing sprite for object class %s"), *ObjectTypeManager.GetName(m_type));
		Daylon::Install<SDaylonSprite>(Sprite, 0.5f); 
	} 
}


void Defcon::IGameObject::UninstallSprite() 
{
	if(Sprite) 
	{
		UE_LOG(LogGame, Log, TEXT("Uninstalling sprite for object class %s"), *ObjectTypeManager.GetName(m_type));
		Daylon::Uninstall(Sprite); 
	} 
}


// -----------------------------------------------------

Defcon::CGameObjectCollection::CGameObjectCollection()
	:
	m_pFirst(nullptr),
	m_pLast(nullptr),
	m_count(0)
{
}


Defcon::CGameObjectCollection::~CGameObjectCollection()
{
	this->DeleteAll();
}


void Defcon::CGameObjectCollection::DeleteAll(bool IncludingSprites)
{
	while(m_pFirst != nullptr)
	{
		if(IncludingSprites)
		{
			m_pFirst->UninstallSprite();
		}
			
		this->Delete(m_pFirst);
	}
}


void Defcon::CGameObjectCollection::ForEach(TFunction<void(IGameObject*)> Function) const
{
	auto p = m_pFirst;

	while(p != nullptr)
	{
		Function(p);
			
		p = p->GetNext();
	}
}


void Defcon::CGameObjectCollection::ForEachUntil(TFunction<bool(IGameObject*)> Function) const
{
	auto p = m_pFirst;

	while(p != nullptr && Function(p))
	{
		p = p->GetNext();
	}
}


void Defcon::CGameObjectCollection::Add(IGameObject* p)
{
	check(p);
	check(p->GetType() != ObjType::UNKNOWN);

	if(m_pFirst != nullptr)
	{
		m_pFirst->SetPrev(p);
		p->SetNext(m_pFirst);
		p->SetPrev(nullptr);
	}
	else
	{
		check(p->GetNext() == nullptr && p->GetPrev() == nullptr);
	}

	m_pFirst = p;
	check(m_pFirst->GetNext() != m_pFirst);

	m_count++;
}


void Defcon::CGameObjectCollection::Add(CGameObjectCollection& set)
{
	IGameObject* p;

	while((p = set.GetFirst()) != nullptr)
	{
		set.Detach(p);
		this->Add(p);
	}
}


void Defcon::CGameObjectCollection::Delete(IGameObject* p)
{
	this->Detach(p);
	if(!p->ExternallyOwned())
	{
		delete p;
	}
}


void Defcon::CGameObjectCollection::Detach(IGameObject* p)
{
	if(p->GetPrev() != nullptr)
		p->GetPrev()->SetNext(p->GetNext());
	else
		m_pFirst = p->GetNext();

	if(p->GetNext() != nullptr)
		p->GetNext()->SetPrev(p->GetPrev());

	p->SetNext(nullptr);
	p->SetPrev(nullptr);
	m_count--;
}


void Defcon::CGameObjectCollection::DetachAll()
{
	while(m_pFirst != nullptr)
		this->Detach(m_pFirst);
}


size_t Defcon::CGameObjectCollection::Count() const
{
	return m_count;
}


size_t Defcon::CGameObjectCollection::CountOf(ObjType Kind) const
{
	size_t Result = 0;

	ForEach([&](IGameObject* p) { if(p->GetType() == Kind) { Result++; } });

	return Result;
}



Defcon::IGameObject* Defcon::CGameObjectCollection::Find(ObjType Kind, Defcon::IGameObject* p) const
{
	if(p == nullptr)
		p = this->GetFirst();
	else
		p = p->GetNext();

	while(p != nullptr)
	{
		if(p->GetType() == Kind)
			break;

		p = p->GetNext();
	}
	
	return p;
}


void Defcon::CGameObjectCollection::Notify(Message Msg, IGameObject* Sender)
{
	ForEach([&](IGameObject* pObj) { pObj->Notify(Msg, Sender); });
}


bool Defcon::CGameObjectCollection::Process(GameObjectProcess& params)
{
	// Perform a standard processing run on all the objects.
	// Return true if any objects were in the collection.

	check(params.pMapper);

	IGameObject* pObj = this->GetFirst();
	bool b = false;

	while(pObj != nullptr)
	{
		b = true;

		if(params.fnOnEvery)
		{
			params.fnOnEvery(pObj, params.pvUser);
		}

		if(pObj->IsMortal() && pObj->ReduceLifespanBy(params.fElapsedTime))
		{
			IGameObject* pObj2 = pObj->GetNext();
#ifdef _DEBUG
			if(!pObj->OccursFrequently())
			{
				char sz[100];
				MySprintf(sz, "Mortal object of class %s deleted\n", pObj->GetClassname());
				OutputDebugString(sz);
			}
#endif
			if(params.fnOnDeath != nullptr)
			{
				params.fnOnDeath(pObj, params.pvUser);
			}

			if(params.UninstallSpriteIfObjectDeleted)
			{
				pObj->UninstallSprite();
			}
			this->Delete(pObj);
			pObj = pObj2;
			continue;
		}
		
		pObj->Move(params.fElapsedTime);

		// Handle wraparound onto planet.
		pObj->m_pos.x = gpArena->WrapX(pObj->m_pos.x);


		// Lots of objects temporarily exist outside arena's vertical bounds.
		//check(pObj->m_pos.y >= 0 && pObj->m_pos.y <= params.fArenaHeight);

		// If the object has a visual, sync its position and update its animation.

		if(pObj->Sprite)
		{
			CFPoint pt;
			params.pMapper->To(pObj->m_pos, pt);
			pObj->Sprite->SetPosition(FVector2D(pt.x, pt.y));
			pObj->Sprite->Update(params.fElapsedTime);
		}

		pObj = pObj->GetNext();
	}
	return b;
}


int Defcon::IGameObject::GetExplosionColorBase() const
{
	return CGameColors::gray;
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


#pragma optimize("", on)
