// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "CoreMinimal.h"
#include "DaylonUtils.h"
#include "GameObjects/GameObjectCollection.h"
#include "GameObjects/playership.h"
#include "Missions/mission.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"

#pragma once

namespace Defcon
{
	struct FTotals
	{
		int32 ShotsFired                   = 0;
		int32 SmartbombsDetonated          = 0;
		int32 HostilesDestroyedBySmartbomb = 0;
		int32 HostilesDestroyedByLaser     = 0;
		int32 FriendlyFireIncidents        = 0;
		int32 PlayerHits                   = 0;
		int32 PlayerCollisions             = 0;
		int32 PlayerDeaths                 = 0;

		void Reset()
		{
			ShotsFired                   = 0;
			SmartbombsDetonated          = 0;
			HostilesDestroyedBySmartbomb = 0;
			HostilesDestroyedByLaser     = 0;
			FriendlyFireIncidents        = 0;
			PlayerHits                   = 0;
			PlayerCollisions             = 0;
			PlayerDeaths                 = 0;
		}
	};


	class CGameSession
	{
		// Tracks the state of a game being played, which starts
		// when the player chooses a mission and ends with the
		// last mission completed or a mission ends but no humans remain.

		public:

			CGameSession  (EMissionID InMissionID);
			~CGameSession ();

			IMission*                    GetMission             () { return Mission; }
			const IMission*              GetMission             () const { return Mission; }
			void                         InitMission            ();
			void                         MissionEnded           ();
			FString                      GetCurrentMissionName  () const;
			void                         TargetDestroyed        (Defcon::EObjType Kind) { if(Mission != nullptr) ((CMilitaryMission*)Mission)->TargetDestroyed(Kind); }

			int32                        GetScore               () const { return Score; }
			void                         SetScore               (int32 Amount) { Score = Amount; }
			int32                        AdvanceScore           (int32 Amount);

			CPlayerShip&                 GetPlayerShip          () { check(PlayerShipPtr != nullptr); return *PlayerShipPtr; }

			CGameObjectCollection&       GetHumans              () { return Humans; }
			const CGameObjectCollection& GetHumans              () const { return Humans; }
			bool                         GetHumansPlaced        () const { return bHumansPlaced; }
			void                         SetHumansPlaced        (bool b = true) { bHumansPlaced = b; }

			bool                         AcquireSmartBomb       ();
			int32                        GetSmartbombCount      () const { return SmartbombsLeft; }
			void                         BindToSmartbombCount   (TFunction<void(const int32& Val)> Delegate) { SmartbombsLeft.Bind(Delegate); }

			FTotals&                     GetStats               () { return Stats; }

			bool                         GetGodMode             () const { return GodMode; }


		protected:

			IMission*                      Mission         = nullptr;
			EMissionID                     MissionID       = EMissionID::First;

			CPlayerShip*                   PlayerShipPtr   = nullptr;
			CGameObjectCollection          Humans;
			bool                           bHumansPlaced   = false;
			
			int32                          Score           = 0;      // Not shown; used only to track XP.
			Daylon::TBindableValue<int32>  SmartbombsLeft;
			bool                           GodMode         = false;

			FTotals                        Stats;
	};

} // namespace Defcon
