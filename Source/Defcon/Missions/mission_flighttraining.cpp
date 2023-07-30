// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	mission_flighttraining.cpp
	Defines flight training mission in Defence Condition.
*/



#include "mission.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "Globals/_sound.h"


#include "GameObjects/bmpdisp.h"
#include "GameObjects/player.h"
#include "GameObjects/beacon.h"
#include "Arenas/DefconPlayViewBase.h"

constexpr int32 NumBeacons = 15;

Defcon::CFlightTrainingMission::CFlightTrainingMission()
{
	ID = MissionID::flighttraining;

	IntroText = 
		"Welcome, Defender.\n"
		"\n"
		"This is a training exercise to help familiarize you\n"
		"with your ship's navigation controls.\n"
		"\n"
		"Press A and Z to move up and down, and K and L to move left and right.\n"
		"\n"
		"Collide with all of the beacon targets to complete the exercise."
		;
}


bool Defcon::CFlightTrainingMission::Update(float fElapsed)
{
	// - Show some intro text.
	// - Make five targets to navigate to.
	// - When all five targets are flown into,
	//   the mission ends.

	if(!IMission::Update(fElapsed))
	{
		return false;
	}

	if(Age < 6.0f)
	{
		this->DoMakeTargets(fElapsed);
		return true;
	}

	if(this->AreAllTargetsHit(fElapsed))
	{
		// We're done.
		return false;
	}

	return true;
}


void Defcon::CFlightTrainingMission::DoMakeTargets(float fElapsed)
{
	if(TargetsMade)
	{
		return;
	}

	TargetsMade = true;

	for(int32 i = 0; i < NumBeacons; i++)
	{
		CBeacon* p = new CBeacon;

		p->InstallSprite();

		p->m_pos.Set(
			MAP(i, 0, 6, gpArena->GetDisplayWidth()*.66f, gpArena->GetWidth() * 0.9f),
			SFRAND * 0.33f * gpArena->GetHeight() + gpArena->GetHeight() / 2);

		gpArena->GetObjects().Add(p);
	}
}


void Defcon::CFlightTrainingMission::CheckTargetHit(float fElapsed)
{
	auto ObjPtr = gpArena->GetObjects().GetFirst();

	while(ObjPtr != nullptr)
	{
		if(ObjPtr->GetType() == ObjType::BEACON)
		{
			auto BeaconPtr = static_cast<CBeacon*>(ObjPtr);
			CFRect rPlayer, rBeacon;

			rPlayer.Set(gpArena->GetPlayerShip().m_pos);
			rPlayer.Inflate(gpArena->GetPlayerShip().m_bboxrad);
			rBeacon.Set(BeaconPtr->m_pos);
			rBeacon.Inflate(BeaconPtr->m_bboxrad);
		
			if(rPlayer.Intersect(rBeacon))
			{
				gpAudio->OutputSound(Defcon::snd_gulp);
				ObjPtr->UninstallSprite();
				gpArena->GetObjects().Delete(ObjPtr);

				const int32 TargetsLeft = gpArena->GetObjects().CountOf(ObjType::BEACON);

				// Show message.
				FString Str;

				if(TargetsLeft > 0)
				{
					auto TargetsDown = NumBeacons - TargetsLeft;

					Str = FString::Printf(TEXT("%d target%s down, %d to go"), TargetsDown, TargetsDown == 1 ? TEXT("") : TEXT("s"), TargetsLeft);
				}
				else
				{
					Str = TEXT("All done! Yay!");
				}

				gpArena->AddMessage(Str);

				break;
			}
		}
		ObjPtr = ObjPtr->GetNext();
	}
}


bool Defcon::CFlightTrainingMission::AreAllTargetsHit(float fElapsed)
{
	this->CheckTargetHit(fElapsed);

	return (gpArena->GetObjects().Find(ObjType::BEACON, nullptr) == nullptr);
}

