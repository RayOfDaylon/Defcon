// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "GameObjectCollection.h"
#include "Arenas/DefconPlayViewBase.h"



Defcon::CGameObjectCollection::CGameObjectCollection()
	:
	First(nullptr),
	NumElems(0)
{
}


Defcon::CGameObjectCollection::~CGameObjectCollection()
{
	this->DeleteAll();
}


void Defcon::CGameObjectCollection::DeleteAll(bool IncludingSprites)
{
	while(First != nullptr)
	{
		if(IncludingSprites)
		{
			First->UninstallSprite();
		}
			
		this->Delete(First);
	}
}


void Defcon::CGameObjectCollection::ForEach(TFunction<void(IGameObject*)> Function) const
{
	auto p = First;

	while(p != nullptr)
	{
		Function(p);
			
		p = p->GetNext();
	}
}


void Defcon::CGameObjectCollection::ForEachUntil(TFunction<bool(IGameObject*)> Function) const
{
	auto p = First;

	while(p != nullptr && Function(p))
	{
		p = p->GetNext();
	}
}


void Defcon::CGameObjectCollection::Add(IGameObject* p)
{
	check(p);
	check(p->GetType() != EObjType::UNKNOWN);

	if(First != nullptr)
	{
		First->SetPrev(p);
		p->SetNext(First);
		p->SetPrev(nullptr);
	}
	else
	{
		check(p->GetNext() == nullptr && p->GetPrev() == nullptr);
	}

	First = p;
	check(First->GetNext() != First);

	NumElems++;
}


void Defcon::CGameObjectCollection::Add(CGameObjectCollection& Set)
{
	IGameObject* p;

	while((p = Set.GetFirst()) != nullptr)
	{
		Set.Detach(p);
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
		First = p->GetNext();
	}

	if(p->GetNext() != nullptr)
	{
		p->GetNext()->SetPrev(p->GetPrev());
	}

	p->SetNext(nullptr);
	p->SetPrev(nullptr);

	NumElems--;
}


void Defcon::CGameObjectCollection::DetachAll()
{
	while(First != nullptr)
	{
		this->Detach(First);
	}
}


int32 Defcon::CGameObjectCollection::Count() const
{
	return NumElems;
}


int32 Defcon::CGameObjectCollection::CountOf(EObjType Kind) const
{
	int32 Result = 0;

	ForEach([&](IGameObject* p) { if(p->GetType() == Kind) { Result++; } });

	return Result;
}


Defcon::IGameObject* Defcon::CGameObjectCollection::Find(EObjType Kind, Defcon::IGameObject* p) const
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


void Defcon::CGameObjectCollection::Notify(EMessage Msg, IGameObject* Sender)
{
	ForEach([&](IGameObject* pObj) { pObj->Notify(Msg, Sender); });
}


bool Defcon::CGameObjectCollection::Process(GameObjectProcessingParams& params)
{
	// Perform a standard processing run on all the objects.
	// Return true if any objects were in the collection.

	check(params.MapperPtr);

	IGameObject* pObj = this->GetFirst();
	bool b = false;

	while(pObj != nullptr)
	{
		b = true;

		if(params.OnEvery)
		{
			params.OnEvery(pObj);
		}

		if(pObj->IsMortal() && pObj->ReduceLifespanBy(params.DeltaTime))
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
			if(params.OnDeath != nullptr)
			{
				params.OnDeath(pObj);
			}

			if(params.UninstallSpriteIfObjectDeleted)
			{
				pObj->UninstallSprite();
			}

			this->Delete(pObj);
			pObj = pObj2;

			continue;
		}
		
		pObj->Move(params.DeltaTime);

		// Handle wraparound onto planet.
		pObj->Position.x = gpArena->WrapX(pObj->Position.x);


		// Lots of objects temporarily exist outside arena's vertical bounds.
		//check(pObj->Position.y >= 0 && pObj->Position.y <= params.ArenaHeight);

		// If the object has a visual, sync its position and update its animation.

		if(pObj->Sprite)
		{
			CFPoint pt;
			params.MapperPtr->To(pObj->Position, pt);
			pObj->Sprite->SetPosition(FVector2D(pt.x, pt.y));
			pObj->Sprite->Update(params.DeltaTime);
		}

		pObj = pObj->GetNext();
	}

	return b;
}
