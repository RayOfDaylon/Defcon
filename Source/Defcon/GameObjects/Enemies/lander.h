// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CLander : public CEnemy
	{
		typedef CEnemy Super;

		public:
			CLander();
			virtual ~CLander();


			virtual void   OnFinishedCreating     (const Daylon::FMetadata& Options) override;
			virtual void   Tick                   (float DeltaTime) override;
			virtual EColor GetExplosionColorBase  () const override;
			virtual void   Notify                 (Defcon::EMessage, void*) override;
			virtual void   OnAboutToDie           () override;

			void           SetDoTryToAbduct       (bool b) { bTryToAbduct = b; }

			CGameObjectCollection*  Objects = nullptr;


		protected:

			void ConsiderFiringBullet (float DeltaTime);


		private:

			enum class EState { Descending, Hovering, Acquiring, Ascending, Ascended, Fighting };

			IGameObject*  Abductee        = nullptr;
			IGameObject*  AbductionTarget = nullptr;
			float         MaxSpeed;
			float         HoverAltitude; // Distance above ground, not absolute Y value.
			float         DescentSpeed;
			float         AscentSpeed;
			float         FiringCountdown;
			EState        State;
			bool          bTryToAbduct;
			bool          bAscendStraight;
	};
}
