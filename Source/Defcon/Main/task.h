// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/util_core.h"
#include "Common/util_geom.h"
#include "GameObjects/obj_types.h"
#include "Common/variant.h"



namespace Defcon
{
	class CEnemy;



	class CRestartMissionTask : public Daylon::IScheduledTask
	{
		public:
			virtual void Do() override;
	};


	class CEndMissionTask : public Daylon::IScheduledTask
	{
		public:
			virtual void Do() override;
	};



	class CCreateGameObjectTask : public Daylon::IScheduledTask
	{
		public:

			CCreateGameObjectTask() {}

			CFPoint            Where;
			Daylon::FMetadata  Options;
			EObjType           ObjType        = EObjType::UNKNOWN;
			EObjType           CreatorType    = EObjType::UNKNOWN;
			bool               bMissionTarget = false;

			virtual void Do() override;

		protected:

			CEnemy* InstantiateGameObject       ();

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


