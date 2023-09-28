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
#include "GameObjects/playership.h"
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


bool Defcon::CFlightTrainingMission::Tick(float DeltaTime)
{
	if(!IMission::Tick(DeltaTime))
	{
		return false;
	}

	if(!TargetsMade)
	{
		DoMakeTargets(DeltaTime);
		TargetsMade = true;
		return true;
	}

	CheckTargetCollided(DeltaTime);

	if(AreAllTargetsCollided())
	{
		// We're done.
		return false;
	}

	return true;
}


void Defcon::CFlightTrainingMission::DoMakeTargets(float DeltaTime)
{
	for(int32 Index = 0; Index < NumBeacons; Index++)
	{
		CBeacon* Beacon = new CBeacon;

		Beacon->InstallSprite();

		Beacon->Position.Set(
			MAP(Index, 0, 6, GArena->GetDisplayWidth() * 0.66f, GArena->GetWidth() * 0.9f),
			SFRAND * 0.33f * GArena->GetHeight() + GArena->GetHeight() / 2);

		GArena->GetObjects().Add(Beacon);
	}

	NumTargetsRemaining = NumBeacons;
}


void Defcon::CFlightTrainingMission::CheckTargetCollided(float DeltaTime)
{
	CFRect rPlayer;
	rPlayer.Set(GArena->GetPlayerShip().Position);
	rPlayer.Inflate(GArena->GetPlayerShip().BboxRadius);

	auto Obj = GArena->GetObjects().GetFirst();

	while(Obj != nullptr)
	{
		if(Obj->GetType() == EObjType::BEACON)
		{
			auto BeaconPtr = static_cast<CBeacon*>(Obj);
			
			CFRect rBeacon;

			rBeacon.Set(BeaconPtr->Position);
			rBeacon.Inflate(BeaconPtr->BboxRadius);
		
			if(rPlayer.Intersect(rBeacon))
			{
				GAudio->OutputSound(EAudioTrack::Gulp);
				Obj->UninstallSprite();
				GArena->GetObjects().Delete(Obj);
				NumTargetsRemaining--;

				// Show message.
				FString Str;

				if(NumTargetsRemaining > 0)
				{
					auto TargetsDown = NumBeacons - NumTargetsRemaining;

					Str = FString::Printf(TEXT("%d target%s down, %d to go"), TargetsDown, TargetsDown == 1 ? TEXT("") : TEXT("s"), NumTargetsRemaining);
				}
				else
				{
					Str = TEXT("All done! Yay!");
				}

				Defcon::GMessageMediator.TellUser(Str, 0.0f, EDisplayMessage::TargetsRemainingChanged);

				break;
			}
		}
		Obj = Obj->GetNext();
	}
}


bool Defcon::CFlightTrainingMission::AreAllTargetsCollided() const
{
	return (NumTargetsRemaining == 0);
}

