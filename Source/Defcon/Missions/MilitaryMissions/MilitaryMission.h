#pragma once

#include "Missions/mission.h"


namespace Defcon
{
	// todo: might be a better way to define missions than as classes. We're repeating a lot of class defs here.

#define SPAWN_ENEMIES(JFactor, MinAlt, MaxAlt)	\
	{	\
	size_t i, j;	\
	for(i = 0; i < array_size(waves); i++)	\
	{	\
		for(j = 0; j < waves[i].count[m_nAttackWave] && this->HostilesRemaining() > 0; j++)	\
		{	\
			float wp = m_pArena->GetWidth();	\
			float x = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * wp + where.x;	\
			x = (float)fmod(x, wp);	\
			float y;	\
			switch(waves[i].type)	\
			{	\
				case ObjType::LANDER:	\
				case ObjType::BOUNCER:	\
					y = FRANDRANGE(0.85f, 1.0f);	\
					break;	\
					\
				default:	\
					y = FRANDRANGE(MinAlt, MaxAlt);	\
			}	\
			y *= m_pArena->GetHeight();	\
			m_pArena->CreateEnemy(waves[i].type, CFPoint(x, y), FRANDRANGE(0.0f, JFactor * j), true, true);	\
		}	\
	}	\
	}

	#define STANDARD_ENEMY_SPAWNING(JFactor)  SPAWN_ENEMIES(JFactor, 0.2f, 0.8f)


	class CMilitaryMission : public IMission
	{
		public:
			CMilitaryMission() {}

			virtual bool    IsMilitary          () const { return true; }

			virtual void	Init				(UDefconPlayViewBase*);
			virtual bool	Update				(float);
			virtual int32	HostilesRemaining	() const;
			virtual int32	HostilesInPlay		() const;
			virtual int32   LandersRemaining    () const { return m_nLandersRemaining; }
			virtual void	MakeTargets			(float, const CFPoint&) = 0;
			virtual void	HostileDestroyed	(ObjType Kind) { m_nHostilesRemaining--; if(Kind == ObjType::LANDER) { m_nLandersRemaining--; } }
			virtual void	AddBaiter			(const CFPoint&);
			virtual void	AddMissionCleaner	(const CFPoint&);
			virtual void	AddNonMissionTarget	(ObjType, const CFPoint&);
			virtual bool	IsCompleted			() override { return (this->HostilesRemaining() == 0); }
			bool            PlayerInStargate	() const;


		protected:
			void AddStargate		();

			IGameObject* m_pStargate = nullptr;

			int32	m_nHostilesRemaining	= 0;   // Number of mission-critical enemies remaining
			int32   m_nLandersRemaining     = 0;
			float	m_fRepopCounter			= 0.0f;
			int32	m_nAttackWave			= 0;
			float	m_fLastCleaner			= 0.0f; // Last time a cleaner enemy (baiter/phred/etc) was spawned
			float	m_CleanerFreq			= 4.0f;
	};


	class CFirstContactMission : public CMilitaryMission
	{
		public:
			CFirstContactMission() { m_ID = MissionID::firstcontact; }

			virtual void Init    (UDefconPlayViewBase*) override;
			virtual bool Update  (float) override;

			virtual FString GetName() const override { return "First Contact"; }
			virtual FString GetDesc() const override { return "A few landers of the Asanthi Apex arrive"; }

		private:
			void         DoIntroText  (float);
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CReinforcedMission : public CMilitaryMission
	{
		public:
			CReinforcedMission() { m_ID = MissionID::reinforcements; }
			virtual void Init    (UDefconPlayViewBase*) override;
			virtual bool Update  (float) override;

			virtual FString GetName() const override { return "Reinforcements"; }
			virtual FString GetDesc() const override { return "The landers bring some friends to escort them"; }

		private:
			void         DoIntroText  (float);
			virtual void MakeTargets  (float, const CFPoint&) override;
	};


	class CBomberShowdown : public CMilitaryMission
	{
		public:
			CBomberShowdown() { m_ID = MissionID::bomber_showdown; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Bomber Showdown"; }
			virtual FString GetDesc() const { return "Even a few mines will blow your shields away"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CFirebomberShowdown : public CMilitaryMission
	{
		public:
			CFirebomberShowdown() { m_ID = MissionID::firebomber_showdown; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Firebomber Showdown"; }
			virtual FString GetDesc() const { return "These guys are hard to hit, and love to shoot"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&) override;
	};


	class CYllabianDogfight : public CMilitaryMission
	{
		public:
			CYllabianDogfight() { m_ID = MissionID::yllabian_dogfight; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Dogfight"; }
			virtual FString GetDesc() const { return "In space, no one can hear you scream"; }

			virtual void CreateTerrain() {}
			virtual void AddHumanoids() {}
			virtual void Conclude() {}
			virtual bool HumansInvolved() { return false; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CYllabianDogfight2 : public CMilitaryMission
	{
		public:
			CYllabianDogfight2() { m_ID = MissionID::yllabian_dogfight2; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Dogfight #2"; }
			virtual FString GetDesc() const { return "Space gets more crowded"; }

			virtual void CreateTerrain() {}
			virtual void AddHumanoids() {}
			virtual void Conclude() {}
			virtual bool HumansInvolved() { return false; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CYllabianEscort : public CMilitaryMission
	{
		public:
			CYllabianEscort() { m_ID = MissionID::yllabian_escort; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Yllabian Escort"; }
			virtual FString GetDesc() const { return "This ought to be interesting"; }


		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CFirebomberPack : public CMilitaryMission
	{
		public:
			CFirebomberPack() { m_ID = MissionID::firebomber_pack; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Firebomber Pack"; }
			virtual FString GetDesc() const { return "A rogue cluster of firebombers in a tight formation"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};

	class CApexOffensive : public CMilitaryMission
	{
		public:
			CApexOffensive() { m_ID = MissionID::apex_offensive; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Apex Offensive"; }
			virtual FString GetDesc() const { return "The Apex beef up the lander escort to full strength"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CPartyMixMission : public CMilitaryMission
	{
		public:
			CPartyMixMission() { m_ID = MissionID::random; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Party Mix"; }
			virtual FString GetDesc() const { return "Mix it up with a random assortment of Apex enemies"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);

			TArray<ObjType> ChosenEnemyTypes;

			int32 IdxEnemyTypes = 0;

			Wave Waves[1] = { { ObjType::UNKNOWN, { 15, 20, 25, 20 } } };

			ObjType EnemyTypes[15] = 
			{
#if 1
				ObjType::LANDER,
				ObjType::LANDER,
				ObjType::LANDER,
				ObjType::HUNTER,
				ObjType::GUPPY,
				ObjType::GUPPY,
				ObjType::BOMBER,
				ObjType::POD,
				ObjType::SWARMER,
				ObjType::FIREBOMBER_TRUE,
				ObjType::FIREBOMBER_WEAK,
				ObjType::DYNAMO,
				ObjType::DYNAMO,
				ObjType::REFORMER,
#endif
				ObjType::GHOST
				//ObjType::SPACEHUM,
				//ObjType::REFORMERPART,
				//ObjType::BAITER,
				//ObjType::PHRED,
				//ObjType::BIGRED,
				//ObjType::MUNCHIE,
			};
	};


	class CApexOffensiveLite : public CMilitaryMission
	{
		public:
			CApexOffensiveLite() { m_ID = MissionID::apex_offensive_lite; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Apex Offensive Lite"; }
			virtual FString GetDesc() const { return "The Apex beef up their lander escort a little more"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


#if 0
	class CSwarm : public CMilitaryMission
	{
		public:
			CSwarm() { m_ID = MissionID::swarm; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Swarm"; }
			virtual FString GetDesc() const { return "A huge swarmer fleet takes you on by itself"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};
#endif

	class CLanderOverrun : public CMilitaryMission
	{
		public:
			CLanderOverrun() { m_ID = MissionID::lander_overrun; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Lander Overrun"; }
			virtual FString GetDesc() const { return "A large group of landers takes matters into their own hands"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CReformerShowdown : public CMilitaryMission
	{
		public:
			CReformerShowdown() { m_ID = MissionID::reformer_showdown; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Reformer Showdown"; }
			virtual FString GetDesc() const { return "Death is only the beginning"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CHaunted : public CMilitaryMission
	{
		public:
			CHaunted() { m_ID = MissionID::haunted; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Haunted"; }
			virtual FString GetDesc() const { return "Landers try a little supernatural help"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CBouncersMission : public CMilitaryMission
	{
		public:
			CBouncersMission() { m_ID = MissionID::bouncers; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Attack of the Bouncers"; }
			virtual FString GetDesc() const { return "Follow the bouncing ball and fire"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CGhostMission : public CMilitaryMission
	{
		public:
			CGhostMission() { m_ID = MissionID::ghost; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Ghost in the Machine"; }
			virtual FString GetDesc() const { return "These reformer mutations are also hard to kill"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};


	class CBringItOn : public CMilitaryMission
	{
		public:
			CBringItOn() { m_ID = MissionID::bring_it_on; }
			virtual void Init(UDefconPlayViewBase*);
			virtual bool Update(float);

			virtual FString GetName() const { return "Bring it On!"; }
			virtual FString GetDesc() const { return "You guys want a piece of me? Do you? Yeah?"; }

		private:
			void DoIntroText(float);
			virtual void MakeTargets(float, const CFPoint&);
	};



	class CMissionFactory
	{
		// The mission factory creates the desired mission. 
		// The mission enums are sorted in chrono order.

		public:
			static IMission* Make(MissionID e)
			{
				switch(e)
				{
					case MissionID::flighttraining: 		return new CFlightTrainingMission; 
					case MissionID::weapons_training:		return new CWeaponsTrainingMission; 
					case MissionID::firstcontact:			return new CFirstContactMission; 
					case MissionID::bomber_showdown:		return new CBomberShowdown;
					case MissionID::reinforcements:			return new CReinforcedMission; 
					case MissionID::yllabian_dogfight:		return new CYllabianDogfight;
					case MissionID::apex_offensive_lite:	return new CApexOffensiveLite;
					//case MissionID::swarm:					return new CSwarm;
					case MissionID::apex_offensive:			return new CApexOffensive;
					case MissionID::firebomber_showdown:	return new CFirebomberShowdown;
					case MissionID::lander_overrun:			return new CLanderOverrun;
					case MissionID::firebomber_pack:		return new CFirebomberPack;
					case MissionID::reformer_showdown:		return new CReformerShowdown;
					case MissionID::yllabian_dogfight2:		return new CYllabianDogfight2;
					case MissionID::ghost:					return new CGhostMission;
					case MissionID::random:					return new CPartyMixMission;
					case MissionID::bring_it_on:			return new CBringItOn;
					case MissionID::haunted:				return new CHaunted;
					case MissionID::bouncers:				return new CBouncersMission;
					case MissionID::yllabian_escort:		return new CYllabianEscort;
				}
				return nullptr;
			}

			static IMission* MakeNext(IMission* pCurrent)
			{
				// nullptr is returned if there is no next mission.
				return CMissionFactory::Make((MissionID)((int)pCurrent->GetID() + 1));
			}
	};
}
