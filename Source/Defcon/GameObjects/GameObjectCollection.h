// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "obj_types.h"
#include "gameobj.h"


namespace Defcon
{
	class IGameObject;
	class GameObjectProcessingParams;

	const bool kIncludingSprites = true;
	const bool kExcludingSprites = !kIncludingSprites;

	class CGameObjectCollection
	{
		// A set of game objects.
		// This handles managing the linked list.

		public:
			CGameObjectCollection();
			virtual ~CGameObjectCollection();

			virtual void Add          (IGameObject*);
			virtual void Add          (CGameObjectCollection&);
			virtual void Delete       (IGameObject*);
			virtual void DeleteAll    (bool IncludingSprites = false);
			virtual void Detach       (IGameObject*);
			virtual void DetachAll    ();
			virtual void ForEach      (TFunction<void(IGameObject*)> Function) const;
			virtual void ForEachUntil (TFunction<bool(IGameObject*)> Function) const;

			virtual int32 Count       () const;
			virtual int32 CountOf     (EObjType Kind) const;
									  
			IGameObject* GetFirst     () const { return First; }
			IGameObject* Find         (EObjType Kind, IGameObject* SearchAfter = nullptr) const;
									  
			virtual bool Process      (GameObjectProcessingParams&);
			virtual void Notify       (EMessage Msg, IGameObject* Sender);

		private:
			IGameObject*    First     = nullptr;
			int32           NumElems  = 0;

	}; // CGameObjectCollection



	typedef std::function<void(IGameObject*)> GameObjectProcessDelegate;
	typedef std::function<void()>             PostDeathDelegate;

	class GameObjectProcessingParams
	{
		public:
			GameObjectProcessDelegate   OnDeath                        = nullptr;
			PostDeathDelegate           OnPostDeath                    = nullptr;
			GameObjectProcessDelegate   OnEvery                        = nullptr;
			I2DCoordMapper*             MapperPtr                      = nullptr;  
			float                       DeltaTime                      = 0.0f;
			bool                        UninstallSpriteIfObjectDeleted = false;
	};
}
