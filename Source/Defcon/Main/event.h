// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/util_geom.h"
#include "GameObjects/obj_types.h"



namespace Defcon
{
	class CEnemy;

	class CEvent
	{
		public:
			CEvent() {}
			virtual ~CEvent() {}
			virtual void Init() {}

			float	m_when = 0.0f; // todo: s/b a countdown value instead of an absolute timestamp
			CFPoint m_where;

			virtual void Do() = 0;
	};


	class CEventQueue
	{
		// This class holds events to be posted  
		// at some time in the future.

		public:
			virtual ~CEventQueue();

			void  Add       (CEvent*);
			void  Process   (); // todo: needs to take DeltaTime arg
			void  DeleteAll ();

		private:
			TArray<CEvent*>  Events;
	};


	class CRestartMissionEvent : public CEvent
	{
		public:
			virtual void Do() override;
	};


	class CEndMissionEvent : public CEvent
	{
		public:
			virtual void Do() override;
	};



	class CCreateEnemyEvent : public CEvent
	{
		public:

			CCreateEnemyEvent() 
				:
				m_bMissionTarget(true)
				{}

			ObjType	m_objtype;
			bool	m_bMissionTarget;

			virtual void Do() override;

		protected:

			CEnemy* CreateEnemy                 (ObjType, const CFPoint&);

			void SpecializeForLander			(CEnemy*, const CFPoint&);
			void SpecializeForGuppy				(CEnemy*, const CFPoint&);
			void SpecializeForHunter			(CEnemy*, const CFPoint&);
			void SpecializeForBomber			(CEnemy*, const CFPoint&);
			void SpecializeForFirebomber		(CEnemy*, const CFPoint&);
			void SpecializeForWeakFirebomber	(CEnemy*, const CFPoint&);
			void SpecializeForFireball			(CEnemy*, const CFPoint&);
			void SpecializeForDynamo			(CEnemy*, const CFPoint&);
			void SpecializeForSpacehum			(CEnemy*, const CFPoint&);
			void SpecializeForPod				(CEnemy*, const CFPoint&);
			void SpecializeForSwarmer			(CEnemy*, const CFPoint&);
			void SpecializeForBaiter			(CEnemy*, const CFPoint&);
			void SpecializeForReformer			(CEnemy*, const CFPoint&);
			void SpecializeForReformerPart		(CEnemy*, const CFPoint&);
			void SpecializeForBouncer			(CEnemy*, const CFPoint&);
			void SpecializeForWeakBouncer		(CEnemy*, const CFPoint&);
			void SpecializeForPhred				(CEnemy*, const CFPoint&);
			void SpecializeForBigRed			(CEnemy*, const CFPoint&);
			void SpecializeForMunchie			(CEnemy*, const CFPoint&);
			void SpecializeForGhost				(CEnemy*, const CFPoint&);
			void SpecializeForGhostPart			(CEnemy*, const CFPoint&);

			void SpecializeForTurret			(CEnemy*, const CFPoint&);
	};
}


