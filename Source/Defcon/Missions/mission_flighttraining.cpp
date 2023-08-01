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


void Defcon::CFlightTrainingMission::Init()
{
	IMission::Init();

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


bool Defcon::CFlightTrainingMission::Update(float DeltaTime)
{
	if(!IMission::Update(DeltaTime))
	{
		return false;
	}

	if(!TargetsMade)
	{
		this->DoMakeTargets(DeltaTime);
		TargetsMade = true;
		return true;
	}

	CheckTargetHit(DeltaTime);

	if(this->AreAllTargetsHit())
	{
		// We're done.
		return false;
	}

	return true;
}


void Defcon::CFlightTrainingMission::DoMakeTargets(float DeltaTime)
{
	for(int32 i = 0; i < NumBeacons; i++)
	{
		CBeacon* Beacon = new CBeacon;

		Beacon->InstallSprite();

		Beacon->Position.Set(
			MAP(i, 0, 6, gpArena->GetDisplayWidth()*.66f, gpArena->GetWidth() * 0.9f),
			SFRAND * 0.33f * gpArena->GetHeight() + gpArena->GetHeight() / 2);

		gpArena->GetObjects().Add(Beacon);
	}

	NumTargets = NumBeacons;
}


void Defcon::CFlightTrainingMission::CheckTargetHit(float DeltaTime)
{
	auto ObjPtr = gpArena->GetObjects().GetFirst();

	CFRect rPlayer;
	rPlayer.Set(gpArena->GetPlayerShip().Position);
	rPlayer.Inflate(gpArena->GetPlayerShip().BboxRadius);

	while(ObjPtr != nullptr)
	{
		if(ObjPtr->GetType() == EObjType::BEACON)
		{
			auto BeaconPtr = static_cast<CBeacon*>(ObjPtr);
			
			CFRect rBeacon;

			rBeacon.Set(BeaconPtr->Position);
			rBeacon.Inflate(BeaconPtr->BboxRadius);
		
			if(rPlayer.Intersect(rBeacon))
			{
				gpAudio->OutputSound(EAudioTrack::Gulp);
				ObjPtr->UninstallSprite();
				gpArena->GetObjects().Delete(ObjPtr);
				NumTargets--;

				// Show message.
				FString Str;

				if(NumTargets > 0)
				{
					auto TargetsDown = NumBeacons - NumTargets;

					Str = FString::Printf(TEXT("%d target%s down, %d to go"), TargetsDown, TargetsDown == 1 ? TEXT("") : TEXT("s"), NumTargets);
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


bool Defcon::CFlightTrainingMission::AreAllTargetsHit() const
{
	return (NumTargets == 0);
}

