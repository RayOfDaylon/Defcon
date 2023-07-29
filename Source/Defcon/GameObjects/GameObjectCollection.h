// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "obj_types.h"
#include "gameobj.h"


namespace Defcon
{
	class IGameObject;
	class GameObjectProcessingParams;

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

			virtual size_t Count      () const;
			virtual size_t CountOf    (ObjType Kind) const;
									  
			IGameObject* GetFirst     () const { return m_pFirst; }
			IGameObject* Find         (ObjType Kind, IGameObject* SearchAfter = nullptr) const;
									  
			virtual bool Process      (GameObjectProcessingParams&);
			virtual void Notify       (Message Msg, IGameObject* Sender);

		private:
			IGameObject*    m_pFirst    = nullptr;
			IGameObject*    m_pLast     = nullptr;
			size_t          m_count     = 0;

	}; // CGameObjectCollection



	typedef std::function<void(IGameObject*, void*)> GameObjectProcessDelegate;

	class GameObjectProcessingParams
	{
		public:
			GameObjectProcessDelegate   fnOnDeath                      = nullptr;
			GameObjectProcessDelegate   fnOnEvery                      = nullptr;
			I2DCoordMapper*             pMapper                        = nullptr;  
			void*                       pvUser                         = nullptr;
			float                       fElapsedTime                   = 0.0f;
			float                       fArenaWidth                    = 0.0f;
			float                       fArenaHeight                   = 0.0f;
			bool                        UninstallSpriteIfObjectDeleted = false;
	};
}
