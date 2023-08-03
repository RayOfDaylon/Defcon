// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once 

/*
	mission.h
	IMission interface class
	Defines base mission class in Defence Condition.
	For military missions, please see MilitaryMissions.h
*/

#include "CoreMinimal.h"
#include "Main/event.h"
#include "Arenas/DefconPlayViewBase.h"


namespace Defcon
{	
	class CEnemy;
	class CBeacon;
	class IGameObject;


	struct FEnemySpawnCounts
	{
		EObjType Kind;			// Type of enemy to spawn
		int32	NumPerWave[4];  // How many of said enemy type to spawn in each wave
	};


	enum class EMissionID
	{
		// Mission IDs. The order of the missions 
		// is determined by the order they are listed here.
		FlightTraining = 0,
		First = FlightTraining,
		WeaponsTraining,
		//HumansTraining,

		FirstContact,
		Reinforcements,
		YllabianDogfight,
		ApexOffensiveLite,
		BomberShowdown,
		//swarm,
		ApexOffensive,
		FirebomberShowdown,
		LanderOverrun,
		FirebomberPack,
		ReformerShowdown,
		YllabianDogfight2,
		Ghost,
		Random,
		BringItOn,
		Haunted,
		Bouncers,
		YllabianEscort,

		//... todo: add the rest
		Count,
		Undefined
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
			IMission();
			virtual ~IMission();

			virtual void Init();

			virtual FString GetName() const = 0;
			virtual FString GetDesc() const = 0;

			virtual bool    IsMilitary      () const { return false; }
			virtual bool	Update			(float DeltaTime);
			virtual void	Conclude		();
			virtual void	CreateTerrain	();
			virtual void	AddHumanoids	();
			virtual bool	HumansInvolved	() const { return true; }
			virtual bool	IsCompleted		() const { return true; }

			EMissionID		GetID			() const { return ID; }
			void			AddEvent		(CEvent* p) { Events.Add(p); }
			bool			IsRunning		() const { return (GArena != nullptr); }

			const FString&  GetIntroText    () const { return IntroText; }


		protected:

			void DoIntroText(float DeltaTime);

			CEventQueue            Events;
			FString                IntroText;
			EMissionID             ID          = EMissionID::Undefined;
			float                  Age         = 0.0f;
			bool                   IntroIsDone = false;
	};


	class CFlightTrainingMission : public IMission
	{
		public:
			CFlightTrainingMission() { ID = EMissionID::FlightTraining; }
			virtual void Init() override;
			virtual bool Update(float) override;

			virtual FString GetName() const override { return "Flight Training"; }
			virtual FString GetDesc() const override { return "Practice flying your new ship"; }

			virtual void AddHumanoids	() override {}
			virtual void Conclude		() override {}
			virtual bool HumansInvolved	() const override { return false; }

		private:
			void DoMakeTargets		(float);
			void CheckTargetHit		(float);
			bool AreAllTargetsHit	() const;

			bool  TargetsMade = false;
			int32 NumTargets = 0;
	};


	class CWeaponsTrainingMission : public IMission
	{
		public:
			CWeaponsTrainingMission() { ID = EMissionID::WeaponsTraining; }
			virtual void Init() override;
			virtual bool Update(float) override;

			virtual FString GetName        () const override { return "Weapons Training"; }
			virtual FString GetDesc        () const override { return "Practice shooting at various targets"; }
			virtual void    AddHumanoids   () override {}
			virtual void    Conclude       () override {}
			virtual bool    HumansInvolved () const override { return false; }

		private:
			void DoMakeTargets		(float);
			bool AreAllTargetsHit	(float);

			bool  TargetsMade = false;
			int32 NumTargetsHit = 0;
	};
}
