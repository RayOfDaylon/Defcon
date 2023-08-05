// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "enemies.h"


namespace Defcon
{
	class CLander : public CEnemy
	{
		public:
			CLander();
			virtual ~CLander();


			virtual void   Move                   (float DeltaTime) override;
			virtual EColor GetExplosionColorBase  () const override;
			virtual void   Notify                 (Defcon::EMessage, void*) override;
			virtual void   OnAboutToDie           () override;

			void SetDoChaseHumans                 (bool b) { bChaseNearestHuman = b; }

			CGameObjectCollection*  Objects = nullptr;


		protected:

			void ConsiderFiringBullet(float DeltaTime);


		private:

			enum class EState { Descending, Hovering, Acquiring, Ascending, Ascended, Fighting };

			IGameObject*  HumanPtr        = nullptr;
			IGameObject*  TrackedHumanPtr = nullptr;
			float         MaxSpeed;
			float         HoverAltitude; // Distance above ground, not absolute Y value.
			float         DescentSpeed;
			float         FiringCountdown;
			EState        State;
			bool          bChaseNearestHuman;
			bool          bAscendStraight;
	};
}
