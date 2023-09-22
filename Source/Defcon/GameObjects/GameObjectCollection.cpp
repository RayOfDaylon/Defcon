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
	DeleteAll();
}


void Defcon::CGameObjectCollection::DeleteAll(bool IncludingSprites)
{
	while(First != nullptr)
	{
		if(IncludingSprites)
		{
			First->UninstallSprite();
		}
			
		Delete(First);
	}
}


void Defcon::CGameObjectCollection::ForEach(GameObjectProcessDelegate Function) const
{
	auto Obj = First;

	while(Obj != nullptr)
	{
		Function(Obj);
			
		Obj = Obj->GetNext();
	}
}


void Defcon::CGameObjectCollection::ForEachUntil(TFunction<bool(IGameObject*)> Function) const
{
	auto Obj = First;

	while(Obj != nullptr && Function(Obj))
	{
		Obj = Obj->GetNext();
	}
}


void Defcon::CGameObjectCollection::Add(IGameObject* Obj)
{
	check(Obj);
	check(Obj->GetType() != EObjType::UNKNOWN);
	check(Obj->Position.IsValid());
	check(Obj->Orientation.Fwd.IsValid());
	check(Obj->Orientation.Up.IsValid());

	if(First != nullptr)
	{
		First->SetPrev(Obj);
		Obj->SetNext(First);
		Obj->SetPrev(nullptr);
	}
	else
	{
		check(Obj->GetNext() == nullptr && Obj->GetPrev() == nullptr);
	}

	First = Obj;
	check(First->GetNext() != First);

	NumElems++;
}


#if 0
void Defcon::CGameObjectCollection::Add(CGameObjectCollection& Set)
{
	IGameObject* Obj;

	while((Obj = Set.GetFirst()) != nullptr)
	{
		Set.Detach(Obj);
		Add(Obj);
	}
}
#endif


void Defcon::CGameObjectCollection::Delete(IGameObject* Obj)
{
	check(Obj);

	Detach(Obj);

	delete Obj;
}


void Defcon::CGameObjectCollection::Detach(IGameObject* Obj)
{
	check(Obj);

	if(Obj->GetPrev() != nullptr)
	{
		Obj->GetPrev()->SetNext(Obj->GetNext());
	}
	else
	{
		First = Obj->GetNext();
	}

	if(Obj->GetNext() != nullptr)
	{
		Obj->GetNext()->SetPrev(Obj->GetPrev());
	}

	Obj->SetNext(nullptr);
	Obj->SetPrev(nullptr);

	NumElems--;
}


void Defcon::CGameObjectCollection::DetachAll()
{
	while(First != nullptr)
	{
		Detach(First);
	}
}


int32 Defcon::CGameObjectCollection::Count() const
{
	return NumElems;
}


int32 Defcon::CGameObjectCollection::CountOf(EObjType Kind) const
{
	int32 Result = 0;

	ForEach([&](IGameObject* Obj) 
	{
		if(Obj->GetType() == Kind) 
		{
			Result++; 
		} 
	});

	return Result;
}


Defcon::IGameObject* Defcon::CGameObjectCollection::Find(EObjType Kind, Defcon::IGameObject* Obj) const
{
	if(Obj == nullptr)
	{
		Obj = GetFirst();
	}
	else
	{
		Obj = Obj->GetNext();
	}

	while(Obj != nullptr)
	{
		if(Obj->GetType() == Kind)
		{
			break;
		}

		Obj = Obj->GetNext();
	}
	
	return Obj;
}


void Defcon::CGameObjectCollection::Notify(EMessage Msg, IGameObject* Sender)
{
	ForEach([&](IGameObject* Obj) 
	{
		Obj->Notify(Msg, Sender); 
	});
}


void Defcon::CGameObjectCollection::Process(GameObjectProcessingParams& Params)
{
	// Perform a standard processing run on all the objects.
	// Return true if any objects were in the collection.

	check(Params.MapperPtr);

	IGameObject* Obj = GetFirst();

	while(Obj != nullptr)
	{
		if(Params.OnEvery)
		{
			Params.OnEvery(Obj);
		}

		if(Obj->IsMortal() && Obj->ReduceLifespanBy(Params.DeltaTime))
		{
			IGameObject* Obj2 = Obj->GetNext();
			
#ifdef TRACE_GAMEOBJ_DEATHS
			if(!Obj->OccursFrequently())
			{
				UE_LOG(LogGame, Log, "Mortal object of class %s deleted\n", *GObjectTypeManager.GetName(Obj->GetType());
			}
#endif
			if(Params.OnDeath != nullptr)
			{
				Params.OnDeath(Obj);
			}

			if(Params.UninstallSpriteIfObjectDeleted)
			{
				Obj->UninstallSprite();
			}

			Delete(Obj);
			Obj = Obj2;

			if(Params.OnPostDeath != nullptr)
			{
				Params.OnPostDeath();
			}

			continue;
		}
		
		Obj->Tick(Params.DeltaTime);

		check(Obj->Position.IsValid());

		Obj->Position.x = GArena->WrapX(Obj->Position.x);


		// Lots of objects temporarily exist outside arena's vertical bounds.
		//check(Obj->Position.y >= 0 && Obj->Position.y <= Params.ArenaHeight);

		// If the object has a visual, sync its position and update its animation.

		if(Obj->Sprite)
		{
			CFPoint P;
			Params.MapperPtr->To(Obj->Position, P);
			Obj->Sprite->SetPosition(FVector2D(P.x, P.y));
			Obj->Sprite->Update(Params.DeltaTime);
		}

		Obj = Obj->GetNext();
	}
}
