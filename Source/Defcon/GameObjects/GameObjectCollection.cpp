// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "GameObjectCollection.h"
#include "Arenas/DefconPlayViewBase.h"



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
	{
		p->GetPrev()->SetNext(p->GetNext());
	}
	else
	{
		m_pFirst = p->GetNext();
	}

	if(p->GetNext() != nullptr)
	{
		p->GetNext()->SetPrev(p->GetPrev());
	}

	p->SetNext(nullptr);
	p->SetPrev(nullptr);

	m_count--;
}


void Defcon::CGameObjectCollection::DetachAll()
{
	while(m_pFirst != nullptr)
	{
		this->Detach(m_pFirst);
	}
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
	{
		p = this->GetFirst();
	}
	else
	{
		p = p->GetNext();
	}

	while(p != nullptr)
	{
		if(p->GetType() == Kind)
		{
			break;
		}

		p = p->GetNext();
	}
	
	return p;
}


void Defcon::CGameObjectCollection::Notify(Message Msg, IGameObject* Sender)
{
	ForEach([&](IGameObject* pObj) { pObj->Notify(Msg, Sender); });
}


bool Defcon::CGameObjectCollection::Process(GameObjectProcessingParams& params)
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
