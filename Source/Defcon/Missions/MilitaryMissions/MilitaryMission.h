// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Missions/mission.h"

// Headers commonly needed by military missions
#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "Globals/prefs.h"
#include "GameObjects/player.h"
#include "GameObjects/obj_types.h"
#include "Arenas/DefconPlayViewBase.h"



namespace Defcon
{
	// todo: might be a better way to define missions than as classes. We're repeating a lot of class defs here.



	class CMilitaryMission : public IMission
	{
		public:
			CMilitaryMission() {}

			virtual bool    IsMilitary          () const { return true; }

			virtual void	Init				() override;
			virtual bool	Update				(float);
			virtual void	MakeTargets			(float, const CFPoint&) = 0;
			virtual void	TargetDestroyed	(EObjType Kind);
			virtual void	AddNonTarget	(EObjType, const CFPoint&);
			virtual bool	IsCompleted			() const override;
			virtual int32	TargetsRemaining	() const;
			virtual int32	TotalHostilesInPlay		() const;
			virtual int32   LandersRemaining    () const { return NumLandersRemaining; }
			bool            PlayerInStargate	() const;


		protected:

			virtual void    AddBaiter           (const CFPoint&);
			virtual void    AddMissionCleaner   (const CFPoint&);

			void            AddStargate         ();
			void            AddEnemySpawnInfo   (const FEnemySpawnCounts& EnemySpawnCounts);
			void            UpdateWaves         (const CFPoint& Where);

			TArray<FEnemySpawnCounts> EnemySpawnCountsArray;
			int32                     MaxWaves               = 0;
			float                     JFactor                = 0.5f;
			float                     MinSpawnAlt            = 0.2f;
			float                     MaxSpawnAlt            = 0.8f;

			CFRect                    StargateRect;
			IGameObject*              StargatePtr            = nullptr;
						              
			int32                     NumTargetsRemaining   = 0;   // Number of mission-critical enemies remaining
			int32                     NumLandersRemaining    = 0;
			float                     RepopCounter           = 0.0f;
			int32                     WaveIndex	             = 0;
			float                     TimeLastCleanerSpawned = 0.0f;
			float                     CleanerFreq            = 4.0f;
	};


	class CFirstContactMission : public CMilitaryMission
	{
		public:
			CFirstContactMission() { ID = EMissionID::FirstContact; }

			virtual void Init    () override;
			//virtual bool Update  (float) override;

			virtual FString GetName() const override { return "First Contact"; }
			virtual FString GetDesc() const override { return "A few landers of the Asanthi Apex arrive"; }

		private:
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CReinforcedMission : public CMilitaryMission
	{
		public:
			CReinforcedMission() { ID = EMissionID::Reinforcements; }
			virtual void Init    () override;
			//virtual bool Update  (float) override;

			virtual FString GetName() const override { return "Reinforcements"; }
			virtual FString GetDesc() const override { return "The landers bring some friends to escort them"; }

		private:
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CBomberShowdown : public CMilitaryMission
	{
		public:
			CBomberShowdown() { ID = EMissionID::BomberShowdown; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Bomber Showdown"; }
			virtual FString GetDesc() const { return "Even a few mines will blow your shields away"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CFirebomberShowdown : public CMilitaryMission
	{
		public:
			CFirebomberShowdown() { ID = EMissionID::FirebomberShowdown; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Firebomber Showdown"; }
			virtual FString GetDesc() const { return "These guys are hard to hit, and love to shoot"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianDogfight : public CMilitaryMission
	{
		public:
			CYllabianDogfight() { ID = EMissionID::YllabianDogfight; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Dogfight"; }
			virtual FString GetDesc() const { return "In space, no one can hear you scream"; }

			virtual void CreateTerrain() {}
			virtual void AddHumanoids() {}
			virtual void Conclude() {}
			virtual bool HumansInvolved() const override { return false; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianDogfight2 : public CMilitaryMission
	{
		public:
			CYllabianDogfight2() { ID = EMissionID::YllabianDogfight2; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Dogfight #2"; }
			virtual FString GetDesc() const { return "Space gets more crowded"; }

			virtual void CreateTerrain() {}
			virtual void AddHumanoids() {}
			virtual void Conclude() {}
			virtual bool HumansInvolved() const override { return false; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianEscort : public CMilitaryMission
	{
		public:
			CYllabianEscort() { ID = EMissionID::YllabianEscort; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Escort"; }
			virtual FString GetDesc() const { return "This ought to be interesting"; }


		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CFirebomberPack : public CMilitaryMission
	{
		public:
			CFirebomberPack() { ID = EMissionID::FirebomberPack; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Firebomber Pack"; }
			virtual FString GetDesc() const { return "A rogue cluster of firebombers in a tight formation"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};

	class CApexOffensive : public CMilitaryMission
	{
		public:
			CApexOffensive() { ID = EMissionID::ApexOffensive; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Apex Offensive"; }
			virtual FString GetDesc() const { return "The Apex beef up the lander escort to full strength"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CPartyMixMission : public CMilitaryMission
	{
		public:
			CPartyMixMission() { ID = EMissionID::Random; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Party Mix"; }
			virtual FString GetDesc() const { return "Mix it up with a random assortment of Apex enemies"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;

			TArray<EObjType> ChosenEnemyTypes;

			int32 IdxEnemyTypes = 0;


			EObjType EnemyTypes[15] = 
			{
				EObjType::LANDER,
				EObjType::LANDER,
				EObjType::LANDER,
				EObjType::HUNTER,
				EObjType::GUPPY,
				EObjType::GUPPY,
				EObjType::BOMBER,
				EObjType::POD,
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
		public:
			CApexOffensiveLite() { ID = EMissionID::ApexOffensiveLite; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Apex Offensive Lite"; }
			virtual FString GetDesc() const { return "The Apex beef up their lander escort a little more"; }

		private:
			//void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&) override;
	};


#if 0
	class CSwarm : public CMilitaryMission
	{
		public:
			CSwarm() { ID = EMissionID::swarm; }
			virtual void Init();
			virtual bool Update(float);

			virtual FString GetName() const { return "Swarm"; }
			virtual FString GetDesc() const { return "A huge swarmer fleet takes you on by itself"; }

		private:
			//void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};
#endif

	class CLanderOverrun : public CMilitaryMission
	{
		public:
			CLanderOverrun() { ID = EMissionID::LanderOverrun; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Lander Overrun"; }
			virtual FString GetDesc() const { return "A large group of landers takes matters into their own hands"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CReformerShowdown : public CMilitaryMission
	{
		public:
			CReformerShowdown() { ID = EMissionID::ReformerShowdown; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Reformer Showdown"; }
			virtual FString GetDesc() const { return "Death is only the beginning"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CHaunted : public CMilitaryMission
	{
		public:
			CHaunted() { ID = EMissionID::Haunted; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Haunted"; }
			virtual FString GetDesc() const { return "Landers try a little supernatural help"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CBouncersMission : public CMilitaryMission
	{
		public:
			CBouncersMission() { ID = EMissionID::Bouncers; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Attack of the Bouncers"; }
			virtual FString GetDesc() const { return "Follow the bouncing ball and fire"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CGhostMission : public CMilitaryMission
	{
		public:
			CGhostMission() { ID = EMissionID::Ghost; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Ghost in the Machine"; }
			virtual FString GetDesc() const { return "These reformer mutations are also hard to kill"; }

		private:
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CBringItOn : public CMilitaryMission
	{
		public:
			CBringItOn() { ID = EMissionID::BringItOn; }
			virtual void Init();
			//virtual bool Update(float);

			virtual FString GetName() const { return "Bring it On!"; }
			virtual FString GetDesc() const { return "You guys want a piece of me? Do you? Yeah?"; }

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
