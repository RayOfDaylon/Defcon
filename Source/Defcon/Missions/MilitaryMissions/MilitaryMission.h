// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Missions/mission.h"

// Headers commonly needed by military missions
#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "Globals/prefs.h"
#include "GameObjects/playership.h"
#include "GameObjects/obj_types.h"
#include "Arenas/DefconPlayViewBase.h"



namespace Defcon
{
	// todo: might be a better way to define missions than as classes. We're repeating a lot of class defs here.


	class CPodIntersectionAlert
	{
		float Countdown      = 0.0f;
		int32 PreviousSecond = 0;

		public:

			CPodIntersectionAlert  (float Time);
			~CPodIntersectionAlert ();

			void  Tick  (float DeltaTime);
			bool  Done  () const { return (Countdown < 0.0f); }
	};


	class CMilitaryMission : public IMission
	{
		typedef IMission Super;

		public:

			CMilitaryMission();
			virtual ~CMilitaryMission();

			virtual bool    IsMilitary          () const override { return true; }

			virtual void    Init                () override;
			virtual bool    Tick                (float DeltaTime) override;
			virtual void    MakeTargets         (float DeltaTime, const CFPoint& Where) { UpdateWaves(Where); }
			virtual void    TargetDestroyed     (EObjType Kind);
			virtual void    AddNonTarget        (EObjType Kind, const CFPoint& P);
			virtual bool    IsMissionComplete   () const override;
			virtual int32   TargetsRemaining    () const;
			virtual int32   TotalHostilesInPlay () const;
			virtual int32   LandersRemaining    () const { return NumLandersRemaining; }
			virtual int32   StartingPodCount    () const { return 0; }
			bool            PlayerInStargate    () const;


		protected:

			virtual void    AddBaiter           (const CFPoint&);
			virtual void    AddMissionCleaner   (const CFPoint&);
			virtual void    OverrideSpawnPoint  (EObjType ObjType, CFPoint& P) {}

			void            AddStargate         ();
			void            AddEnemySpawnInfo   (const FEnemySpawnCounts& EnemySpawnCounts);
			void            UpdateWaves         (const CFPoint& Where);
			void            CancelPodIntersectionAlert();

			CPodIntersectionAlert*    PodIntersectionAlert   = nullptr;

			TArray<FEnemySpawnCounts> EnemySpawnCountsArray;
			int32                     MaxWaves               = 0;
			float                     SpawnTimeFactor        = 0.5f; // Larger values cause higher chance of enemies spawning farther apart in time.
			                                                         // Given N enemies per wave, a factor of 1.0 could make the last enemy take up to N seconds to spawn.
			Daylon::FRange<float>     SpawnAltitudeRange     = Daylon::FRange<float>(0.2f, 0.8f);
			float                     SpawnRangeHorizontal;

			CFRect                    StargateRect;
			IGameObject*              StargatePtr            = nullptr;
									  
			float                     RepopCounter           = 0.0f;
			float                     TimeLastCleanerSpawned = 0.0f;
			float                     CleanerFreq            = 4.0f;
			int32                     NumTargetsRemaining    = 0;   // Number of mission-critical enemies remaining
			int32                     NumLandersRemaining    = 0;
			int32                     WaveIndex              = 0;
	};


	class CFirstContactMission : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CFirstContactMission() { ID = EMissionID::FirstContact; }
			virtual void Init() override;

			virtual FString GetName() const override { return "First Contact"; }
			virtual FString GetDesc() const override { return "A few landers of the Asanthi Apex arrive"; }

		private:
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CReinforcedMission : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CReinforcedMission() { ID = EMissionID::Reinforcements; }
			virtual void Init    () override;

			virtual FString GetName() const override { return "Reinforcements"; }
			virtual FString GetDesc() const override { return "The landers bring some friends to escort them"; }

			virtual int32   StartingPodCount  () const override { return 2; }

		private:
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CBomberShowdown : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CBomberShowdown() { ID = EMissionID::BomberShowdown; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Bomber Showdown"; }
			virtual FString GetDesc() const override { return "Even a few mines will blow your shields away"; }

			virtual int32   StartingPodCount  () const override { return 3; }

	};


	class CFirebomberShowdown : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CFirebomberShowdown() { ID = EMissionID::FirebomberShowdown; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Firebomber Showdown"; }
			virtual FString GetDesc() const override { return "These guys are hard to hit, and love to shoot"; }
	};


	class CYllabianDogfight : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CYllabianDogfight() { ID = EMissionID::YllabianDogfight; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Yllabian Dogfight"; }
			virtual FString GetDesc() const override { return "In space, no one can hear you scream"; }

			virtual bool    UsesTerrain     () const override { return false; }
			virtual bool    HumansInvolved  () const override { return false; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianDogfight2 : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CYllabianDogfight2() { ID = EMissionID::YllabianDogfight2; }
			virtual void Init() override;

			virtual FString GetName         () const override { return "Yllabian Dogfight #2"; }
			virtual FString GetDesc         () const override { return "Space gets more crowded"; }

			virtual bool    UsesTerrain     () const override { return false; }
			virtual bool    HumansInvolved  () const override { return false; }

		protected:
			virtual void    OverrideSpawnPoint  (EObjType ObjType, CFPoint& SpawnPoint);
			virtual int32   StartingPodCount    () const override { return 4; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianEscort : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CYllabianEscort() { ID = EMissionID::YllabianEscort; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Yllabian Escort"; }
			virtual FString GetDesc() const override { return "This ought to be interesting"; }
			virtual int32   StartingPodCount  () const override { return 5; }


		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CFirebomberPack : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CFirebomberPack() { ID = EMissionID::FirebomberPack; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Firebomber Pack"; }
			virtual FString GetDesc() const override { return "A rogue cluster of firebombers in a tight formation"; }
			virtual int32   StartingPodCount  () const override { return 4; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CApexOffensive : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CApexOffensive() { ID = EMissionID::ApexOffensive; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Apex Offensive"; }
			virtual FString GetDesc() const override { return "The Apex beef up the lander escort to full strength"; }
			virtual int32   StartingPodCount  () const override { return 4; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CPartyMixMission : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CPartyMixMission() { ID = EMissionID::Random; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Party Mix"; }
			virtual FString GetDesc() const override { return "Mix it up with a random assortment of Apex enemies"; }
			virtual int32   StartingPodCount  () const override { return 4; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;

			TArray<EObjType> ChosenEnemyTypes;

			int32 IdxEnemyTypes = 0;


			EObjType EnemyTypes[14] = 
			{
				EObjType::LANDER,
				EObjType::LANDER,
				EObjType::LANDER,
				EObjType::HUNTER,
				EObjType::GUPPY,
				EObjType::GUPPY,
				EObjType::BOMBER,
				//EObjType::POD,
				EObjType::SWARMER,
				EObjType::FIREBOMBER_TRUE,
				EObjType::FIREBOMBER_WEAK,
				EObjType::DYNAMO,
				EObjType::DYNAMO,
				EObjType::REFORMER,
				EObjType::GHOST
			};
	};


	class CApexOffensiveLite : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CApexOffensiveLite() { ID = EMissionID::ApexOffensiveLite; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Apex Offensive Lite"; }
			virtual FString GetDesc() const override { return "The Apex beef up their lander escort a little more"; }
			virtual int32   StartingPodCount  () const override { return 3; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


#if 0
	class CSwarm : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CSwarm() { ID = EMissionID::swarm; }
			virtual void Init () override;
			virtual bool Tick (float);

			virtual FString GetName() const { return "Swarm"; }
			virtual FString GetDesc() const { return "A huge swarmer fleet takes you on by itself"; }

		private:
			virtual void MakeTargets(float, const CFPoint&);
	};
#endif

	class CLanderOverrun : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CLanderOverrun() { ID = EMissionID::LanderOverrun; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Lander Overrun"; }
			virtual FString GetDesc() const override { return "A large group of landers takes matters into their own hands"; }
	};


	class CReformerShowdown : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CReformerShowdown() { ID = EMissionID::ReformerShowdown; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Reformer Showdown"; }
			virtual FString GetDesc() const override { return "Death is only the beginning"; }
	};


	class CHaunted : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CHaunted() { ID = EMissionID::Haunted; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Haunted"; }
			virtual FString GetDesc() const override { return "Landers try a little supernatural help"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CBouncersMission : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CBouncersMission() { ID = EMissionID::Bouncers; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Attack of the Bouncers"; }
			virtual FString GetDesc() const override { return "Follow the bouncing ball and fire"; }
			virtual int32   StartingPodCount  () const override { return 4; }


		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CGhostMission : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CGhostMission() { ID = EMissionID::Ghost; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Ghost in the Machine"; }
			virtual FString GetDesc() const override { return "These reformer mutations are also hard to kill"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CBringItOn : public CMilitaryMission
	{
		typedef CMilitaryMission Super;

		public:
			CBringItOn() { ID = EMissionID::BringItOn; }
			virtual void Init() override;

			virtual FString GetName() const override { return "Bring it On!"; }
			virtual FString GetDesc() const override { return "You guys want a piece of me? Do you? Yeah?"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};



	class CMissionFactory
	{
		// The mission factory creates the desired mission. 
		// The mission enums are sorted in chrono order.

		public:
			static IMission* Make(EMissionID e)
			{
				switch(e)
				{
					case EMissionID::ApexOffensive:         return new CApexOffensive;
					case EMissionID::ApexOffensiveLite:     return new CApexOffensiveLite;
					case EMissionID::BomberShowdown:        return new CBomberShowdown;
					case EMissionID::Bouncers:              return new CBouncersMission;
					case EMissionID::BringItOn:             return new CBringItOn;
					case EMissionID::FirebomberPack:        return new CFirebomberPack;
					case EMissionID::FirebomberShowdown:    return new CFirebomberShowdown;
					case EMissionID::FirstContact:          return new CFirstContactMission; 
					case EMissionID::FlightTraining:        return new CFlightTrainingMission; 
					case EMissionID::Ghost:                 return new CGhostMission;
					case EMissionID::Haunted:               return new CHaunted;
					case EMissionID::LanderOverrun:         return new CLanderOverrun;
					case EMissionID::Random:                return new CPartyMixMission;
					case EMissionID::ReformerShowdown:      return new CReformerShowdown;
					case EMissionID::Reinforcements:        return new CReinforcedMission; 
					case EMissionID::WeaponsTraining:       return new CWeaponsTrainingMission; 
					case EMissionID::YllabianDogfight:      return new CYllabianDogfight;
					case EMissionID::YllabianDogfight2:     return new CYllabianDogfight2;
					case EMissionID::YllabianEscort:        return new CYllabianEscort;
					//case EMissionID::Swarm:               return new CSwarm;
				}
				return nullptr;
			}

			static IMission* MakeNext(IMission* pCurrent)
			{
				// nullptr is returned if there is no next mission.
				return CMissionFactory::Make((EMissionID)((int32)pCurrent->GetID() + 1));
			}
	};
}
