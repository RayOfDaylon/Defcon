// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "CoreMinimal.h"
#include "GameObjects/GameObjectCollection.h"
#include "GameObjects/playership.h"
#include "Missions/mission.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"

#pragma once

namespace Defcon
{
	// todo: it's not obvious that match stats should be a member of the match.
	// After a match, the game over view needs to show the stats, so we would 
	// have to copy the stats to somewhere else. Unless we have the game over arena
	// require a current match, so we'd extend the match lifetime to it.
	// But it's easy to imagine other things that would need the stats later, so...


	class CGameMatch
	{
		// Tracks the state of a match being played, which starts when the
		// player chooses a mission and ends with the last mission completed
		// or a mission ends but no humans remain.
		
		// For our purposes, a match ends when the user transitions to an arena 
		// that doesn't require a current match.
		
		// A match holds the player ship and humans, because those objects 
		// persist across missions. The current mission is also a match member.

		public:

			CGameMatch  (EMissionID InMissionID);
			~CGameMatch ();

			bool                          Update                 (float DeltaTime);
										  
			void                          SetCurrentMission      (Defcon::EMissionID InMissionID);
			IMission*                     GetMission             () { return Mission; }
			const IMission*               GetMission             () const { return Mission; }
			void                          InitMission            ();
			void                          MissionEnded           ();
			FString                       GetCurrentMissionName  () const;
			void                          TargetDestroyed        (Defcon::EObjType Kind) { if(Mission != nullptr) ((CMilitaryMission*)Mission)->TargetDestroyed(Kind); }
										  
			int32                         GetScore               () const { return Score; }
			void                          SetScore               (int32 Amount) { Score = Amount; }
			int32                         AdvanceScore           (int32 Amount);
			void                          AdjustScore            (int32 Amount);
										  
			CPlayerShip&                  GetPlayerShip          () { check(PlayerShip != nullptr); return *PlayerShip; }
										  
			CGameObjectCollection&        GetHumans              () { return Humans; }
			const CGameObjectCollection&  GetHumans              () const { return Humans; }
			bool                          GetHumansPlaced        () const { return bHumansPlaced; }
			void                          SetHumansPlaced        (bool b = true) { bHumansPlaced = b; }
										  

			bool                          GetGodMode             () const { return GodMode; }
			void                          SetGodMode             (bool b = true) { GodMode = b; }


		protected:

			IMission*                      Mission         = nullptr;
			EMissionID                     MissionID       = EMissionID::First;

			CPlayerShip*                   PlayerShip      = nullptr;
			CGameObjectCollection          Humans;
			bool                           bHumansPlaced   = false;
			
			int32                          Score           = 0;      // Not shown; used only to track XP.
			bool                           GodMode         = false;
	};


	extern CGameMatch* GGameMatch;
} // namespace Defcon
