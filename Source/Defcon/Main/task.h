// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/util_geom.h"
#include "GameObjects/obj_types.h"



namespace Defcon
{
	class CEnemy;

	class CScheduledTask
	{
		public:
			CScheduledTask() {}
			virtual ~CScheduledTask() {}

			float   Countdown = 0.0f; // If this is not set, task will run immediately.

			virtual void Do() = 0;
	};


	class CScheduledTaskList
	{
		// Tasks to be run at some time in the future.

		public:
			virtual ~CScheduledTaskList();

			void  Add           (CScheduledTask*);
			void  Process       (float DeltaTime);
			void  DeleteAll     ();
			bool  IsEmpty       () const { return Tasks.IsEmpty(); }
			void  ForEach       (TFunction<void(CScheduledTask*)> Function) const;
			void  ForEachUntil  (TFunction<bool(CScheduledTask*)> Function) const;

		private:
			TArray<CScheduledTask*>  Tasks;
	};


	class CRestartMissionTask : public CScheduledTask
	{
		public:
			virtual void Do() override;
	};


	class CEndMissionTask : public CScheduledTask
	{
		public:
			virtual void Do() override;
	};



	class CCreateEnemyTask : public CScheduledTask
	{
		public:

			CCreateEnemyTask() {}

			CFPoint   Where;
			EObjType  EnemyType      = EObjType::UNKNOWN;
			EObjType  CreatorType    = EObjType::UNKNOWN;
			bool      bMissionTarget = false;

			virtual void Do() override;

		protected:

			CEnemy* CreateEnemy                 ();

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

