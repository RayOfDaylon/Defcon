// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once 

/*
	mission.h
	IMission interface class
	Defines base mission class in Defence Condition.
	For military missions, please see MilitaryMissions.h
*/


#include "Main/event.h"
#include "Arenas/DefconPlayViewBase.h"


namespace Defcon
{	
	class CEnemy;
	class CBeacon;
	class IGameObject;


	class Wave
	{
		public:
			ObjType type;
			size_t	count[4];
	};


	enum class MissionID
	{
		// Mission IDs. The order of the missions 
		// is determined by the order they are listed here.
		flighttraining = 0,
		first = flighttraining,
		weapons_training,
		//humanstraining,

		firstcontact,
		bomber_showdown,
		reinforcements,
		yllabian_dogfight,
		apex_offensive_lite,
		//swarm,
		apex_offensive,
		firebomber_showdown,
		lander_overrun,
		firebomber_pack,
		reformer_showdown,
		yllabian_dogfight2,

		ghost,
		random,
		bring_it_on,
		haunted,
		bouncers,
		yllabian_escort,

		//... todo: add the rest
		missions_count,
		notdef
	};


	class IMission
	{
		// A mission is a tight coupler to a play arena.
		// The mission's Update method is called with every 
		// frame, and the mission decides what to do at 
		// what time, such as inserting particular objects 
		// into the arena (e.g., enemies) or changing other 
		// objects and conditions. If Update returns false, 
		// the mission has concluded.

		public:
			IMission() {}
			virtual ~IMission() {}

			virtual void Init(UDefconPlayViewBase*);

			virtual FString GetName() const = 0;
			virtual FString GetDesc() const = 0;

			virtual bool    IsMilitary      () const { return false; }
			virtual bool	Update			(float);
			virtual void	Conclude		();
			virtual bool	HumansInvolved	() { return true; }
			virtual bool	IsCompleted		() { return true; }
			virtual void	CreateTerrain	();
			virtual void	AddHumanoids	();

			MissionID		GetID			() const { return m_ID; }
			void			AddEvent		(CEvent* p) { m_events.Add(p); }
			bool			IsRunning		() const { return (m_pArena != nullptr); }


		protected:

			CEventQueue            m_events;
			MissionID              m_ID         = MissionID::notdef;
			float                  m_fAge       = 0.0f;
			UDefconPlayViewBase*   m_pArena     = nullptr;
			bool                   m_bIntroDone = false;
	};


	class CFlightTrainingMission : public IMission
	{
		public:
			CFlightTrainingMission() { m_ID = MissionID::flighttraining; }
			virtual bool Update(float) override;

			virtual FString GetName() const override { return "Flight Training"; }
			virtual FString GetDesc() const override { return "Practice flying your new ship"; }

			virtual void AddHumanoids	() override {}
			virtual void Conclude		() override {}
			virtual bool HumansInvolved	() override { return false; }

		private:
			void DoIntroText		(float);
			void DoMakeTargets		(float);
			bool AreAllTargetsHit	(float);
			void CheckTargetHit		(float);

			bool m_bTargetsMade = false;
	};


	class CWeaponsTrainingMission : public IMission
	{
		public:
			CWeaponsTrainingMission() { m_ID = MissionID::weapons_training; }
			virtual void Init(UDefconPlayViewBase*) override;
			virtual bool Update(float) override;

			virtual FString GetName        () const override { return "Weapons Training"; }
			virtual FString GetDesc        () const override { return "Practice shooting at various targets"; }
			virtual void    AddHumanoids   () override {}
			virtual void    Conclude       () override {}
			virtual bool    HumansInvolved () override { return false; }

		private:
			void DoIntroText		(float);
			void DoMakeTargets		(float);
			bool AreAllTargetsHit	(float);
			//void CheckTargetHit		(float);

			bool m_bTargetsMade = false;
			int32 NumTargetsHit = 0;
	};
}
