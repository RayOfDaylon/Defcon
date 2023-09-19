// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	MilitaryMission.cpp
	Base class of military missions.
*/


#include "MilitaryMission.h"

#include "DefconUtils.h"

#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "GameObjects/playership.h"
#include "GameObjects/human.h"
#include "GameObjects/Enemies/enemies.h"
#include "GameObjects/Auxiliary/stargate.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




Defcon::CPodIntersectionAlert::CPodIntersectionAlert(float Time)
{
	Countdown = Time;
	/*
	FMessageConsumer MessageConsumer(this, EMessageEx::PodIntersectionStarted, 
		[this](void* Payload)
		{ 
			check(Payload != nullptr);
			Countdown = ((FPodIntersectionInfo*)Payload)->Time;
		});

	GMessageMediator.RegisterConsumer(MessageConsumer);*/
}


Defcon::CPodIntersectionAlert::~CPodIntersectionAlert()
{
	//GMessageMediator.UnregisterConsumer(this);

	auto Text = FText();
	GMessageMediator.Send(EMessageEx::SetTopMessage, &Text);
}


void Defcon::CPodIntersectionAlert::Tick(float DeltaTime)
{
	Countdown -= DeltaTime;

	if(Countdown < 0)
	{
		//PreviousSecond = 0;
		//auto Text = FText();
		//GMessageMediator.Send(EMessageEx::SetTopMessage, &Text);
		return;
	}

	int32 CurrentSecond = (int32)Countdown;

	if(CurrentSecond != PreviousSecond)
	{
		PreviousSecond = CurrentSecond;

		auto Text = FText::FromString(FString::Printf(TEXT("POD INTERSECTION - 0:0%d"), CurrentSecond));

		GMessageMediator.Send(EMessageEx::SetTopMessage, &Text);
	}
}


// ---------------------------------------------------------------------------

Defcon::CMilitaryMission::CMilitaryMission()
{
}


Defcon::CMilitaryMission::~CMilitaryMission()
{
	SAFE_DELETE(PodIntersectionAlert);
}


void Defcon::CMilitaryMission::Init()
{
	Super::Init();

	// Military missions have stargates.
	AddStargate();

	SpawnRangeHorizontal = GArena->GetWidth() * ATTACK_INITIALDISTANCE;

	NumTargetsRemaining = StartingPodCount();
}


void Defcon::CMilitaryMission::AddEnemySpawnInfo(const FEnemySpawnCounts& EnemySpawnCounts)
{
	EnemySpawnCountsArray.Add(EnemySpawnCounts);

	int32 WaveNumber = 1;

	for(auto EnemyTypeCount : EnemySpawnCounts.NumPerWave)
	{
		NumTargetsRemaining += EnemyTypeCount;

		if(EnemySpawnCounts.Kind == EObjType::LANDER)
		{
			NumLandersRemaining	+= EnemyTypeCount;
		}

		if(EnemyTypeCount > 0 && WaveNumber > MaxWaves)
		{
			MaxWaves = WaveNumber;
		}

		WaveNumber++;
	}
}


void Defcon::CMilitaryMission::AddStargate()
{
	check(GArena != nullptr);
	StargatePtr = new CStargate;
	CFPoint pos(0.5f, 0.75f);
	pos.Mul(CFPoint(GArena->GetWidth(), GArena->GetHeight()));
	StargatePtr->Position = pos; 
	StargatePtr->InstallSprite();
	GArena->GetObjects().Add(StargatePtr);

	// Since the stargate is stationary, cache its rectangle.
	StargateRect.Set(StargatePtr->Position);
	StargateRect.Inflate(StargatePtr->BboxRadius);
}


bool Defcon::CMilitaryMission::PlayerInStargate() const
{
	return StargateRect.PtInside(GArena->GetPlayerShip().Position);
}


void Defcon::CMilitaryMission::UpdateWaves(const CFPoint& Where)
{
	// For now, defer to legacy wave processing if this class has empty EnemySpawnCountsArray member.

	if(WaveIndex >= MaxWaves || EnemySpawnCountsArray.IsEmpty())
	{
		return;
	}

	if(    !( (TotalHostilesInPlay() == 0 && RepopCounter > DELAY_BEFORE_ATTACK) 
		   || (TotalHostilesInPlay() > 0 && RepopCounter > DELAY_BETWEEN_REATTACK) ))
	{
		return;
	}

	const float ArenaWidth = GArena->GetWidth();


	// See if we have any pods in the first wave. If so, set up a pod intersection.

	if(WaveIndex == 0)
	{
		const int32 NumPods = StartingPodCount();

		if(NumPods > 1)
		{
			// Spawn the pods so that eventually meet up within smart bomb range.
			// To simplify things, make them all spawn simultaneously.

			// The width of the intersection area depends on XP. The more XP, the wider the area.
			const float XP = FMath::Min(1.0f, GGameMatch->GetScore() / 50000.0f);
		
			const float PodIntersectionWidth = Daylon::Lerp(POD_INTERSECTION_WIDTH, XP) * GArena->GetDisplayWidth();

			// Pick where the intersection will happen. It shouldn't be near the player.
			const float PodIntersectionX = ArenaWidth * FRANDRANGE(0.33f, 0.67f);

			// Pick how long it takes the intersection to form.
			const float PodIntersectionTime = FRANDRANGE(3.0f, 8.0f);

			for(int32 PodIndex = 0; PodIndex < NumPods; PodIndex++)
			{
				// Pick a random place where the pod will end up.
				CFPoint EndSpawnPoint(
					fmod(PodIntersectionX + FRANDRANGE(-PodIntersectionWidth / 2, PodIntersectionWidth / 2), ArenaWidth), 
					GArena->GetHeight() * Daylon::FRandRange(SpawnAltitudeRange));

				// Pick a x-velocity for the pod.
				const float PodSpeedX = Daylon::FRandRange(POD_SPEED);
				const float PodOrientationX = SBRAND;

				// Work backwards to determine starting spawn point.
				CFPoint SpawnPoint = EndSpawnPoint;
				SpawnPoint.x -= PodSpeedX * PodOrientationX * PodIntersectionTime;
				SpawnPoint.x = GArena->WrapX(SpawnPoint.x);


				// Include extra spawn options.
				Daylon::FMetadata Options;

				Daylon::FVariant Value;

				Value.Real = PodOrientationX;
				Options.Map.Add(TEXT("OrientationX"), Value); 

				Value.Real = PodSpeedX;
				Options.Map.Add(TEXT("SpeedX"), Value); 

				GArena->SpawnGameObject(EObjType::POD, EObjType::UNKNOWN, SpawnPoint, 0.0f, EObjectCreationFlags::StandardEnemy, &Options);	
			}

			PodIntersectionAlert = new CPodIntersectionAlert(PodIntersectionTime);

			//FPodIntersectionInfo Payload = { PodIntersectionTime };
			//GMessageMediator.Send(EMessageEx::PodIntersectionStarted, &Payload);
		}
	}

	RepopCounter = 0.0f;
	

	for(int32 EnemyTypeIndex = 0; EnemyTypeIndex < EnemySpawnCountsArray.Num(); EnemyTypeIndex++)	
	{	
		for(int32 SpawnCountIndex = 0; SpawnCountIndex < EnemySpawnCountsArray[EnemyTypeIndex].NumPerWave[WaveIndex]; SpawnCountIndex++)	
		{	
			CFPoint SpawnPoint;

			SpawnPoint.x = FRANDRANGE(-0.5f, 0.5f) * SpawnRangeHorizontal + Where.x;	
			SpawnPoint.x = (float)fmod(SpawnPoint.x, ArenaWidth);	

			switch(EnemySpawnCountsArray[EnemyTypeIndex].Kind)	
			{	
				// Make these enemies spawn in upper half
				case EObjType::LANDER:	
				case EObjType::BOUNCER:	
					SpawnPoint.y = FRANDRANGE(0.5f, 0.95f);	
					break;	
					
				default:	
					SpawnPoint.y = Daylon::FRandRange(SpawnAltitudeRange);
					break;
			}	

			SpawnPoint.y *= GArena->GetHeight();

			OverrideSpawnPoint(EnemySpawnCountsArray[EnemyTypeIndex].Kind, SpawnPoint);

			GArena->SpawnGameObject(EnemySpawnCountsArray[EnemyTypeIndex].Kind, EObjType::UNKNOWN, SpawnPoint, FRANDRANGE(0.0f, SpawnTimeFactor * SpawnCountIndex), EObjectCreationFlags::StandardEnemy);	
		}	
	}	
	
	WaveIndex++;
}


bool Defcon::CMilitaryMission::IsMissionComplete() const
{
	// Report if the mission has ended and the player is allowed to leave.
	// All the waves must have occured, no more mission target creation tasks are pending,
	// and none of the existing enemies must be a mission target.

	if(WaveIndex < MaxWaves)
	{
		return false;
	}


	bool Result = true;

	EnemyCreationTasks.ForEachUntil([&Result](CScheduledTask* Task)
	{
		auto EnemyCreationTask = static_cast<CCreateGameObjectTask*>(Task);
		
		if(!EnemyCreationTask->bMissionTarget)
		{
			return true;
		}
		Result = false;
		return false;
	});

	if(Result == false)
	{
		return false;
	}


	Result = true;

	GArena->GetEnemies().ForEachUntil([&Result](IGameObject* Obj)
	{
		const auto Enemy = static_cast<CEnemy*>(Obj);

		if(!Enemy->IsMissionTarget())
		{
			return true;
		}

		Result = false;
		return false;
	});

	return Result;
}


bool Defcon::CMilitaryMission::Tick(float DeltaTime)
{
	if(!Super::Tick(DeltaTime))
	{
		return false;
	}

	RepopCounter += DeltaTime;

	if(PodIntersectionAlert != nullptr)
	{
		PodIntersectionAlert->Tick(DeltaTime);

		if(PodIntersectionAlert->Done())
		{
			DELETE_AND_NULL(PodIntersectionAlert);
		}
	}

	if(PlayerInStargate())
	{
		GArena->AllStopPlayerShip();

		if(!IsMissionComplete())
		{
			GArena->TransportPlayerShip();
		}
		else
		{
			// Warp to next mission.

			auto& Player = GArena->GetPlayerShip();
			Player.EnableInput(false);

			// If the player is carrying any humans, redistribute them back to the ground to avoid clumping in next mission.
			GGameMatch->GetHumans().ForEach([this](IGameObject* pObj)
			{
				auto Human = static_cast<CHuman*>(pObj);

				if(Human->IsBeingCarried() && Human->GetCarrier()->GetType() == EObjType::PLAYER)
				{
					Human->SetToNotCarried();
					Human->Position.Set(FRANDRANGE(0.0f, GArena->GetWidth() - 1), 0.04f * GArena->GetHeight()); 
				}
			});

 			return false;
		}
	}

	MakeTargets(DeltaTime, GArena->GetPlayerShip().Position);

	return true;
}


void Defcon::CMilitaryMission::TargetDestroyed(EObjType Kind)
{
	check(NumTargetsRemaining > 0);

	NumTargetsRemaining--;

	if(Kind == EObjType::LANDER) 
	{
		check(NumLandersRemaining > 0);

		NumLandersRemaining--; 
	} 
}


int32 Defcon::CMilitaryMission::TargetsRemaining() const
{
	return NumTargetsRemaining;
}


int32 Defcon::CMilitaryMission::TotalHostilesInPlay() const
{
	check(GArena != nullptr);

	return (int32)GArena->GetEnemies().Count();
}


void Defcon::CMilitaryMission::AddMissionCleaner(const CFPoint& where)
{
	// When a mission drags on too long, it's time 
	// to throw in various nonmission hostiles at an increasing 
	// frequency until the player wipes out the critical opponents.

	// Munchies are spawned by phreds.

	const EObjType CleanerTypes[] = { EObjType::PHRED, EObjType::BIGRED /*, EObjType::MUNCHIE*/ };

	if(Age - TimeLastCleanerSpawned >= CleanerFreq)
	{
		if(CleanerFreq > 0.5f)
		{
			CleanerFreq -= 0.5f;
		}

		AddNonTarget(FRAND < BAITER_PROB ? EObjType::BAITER : CleanerTypes[IRAND(array_size(CleanerTypes))], where);
	}
}


void Defcon::CMilitaryMission::AddNonTarget(EObjType ObjType, const CFPoint& Where)
{
	check(GArena != nullptr);

	TimeLastCleanerSpawned = Age;

	const float ArenaWidth = GArena->GetWidth();
	float X = (FRAND - 0.5f) * 1.0f * GArena->GetDisplayWidth() + Where.x;
	X = (float)fmod(X, ArenaWidth);
	float Y = FRANDRANGE(0.3f, 0.8f) * GArena->GetHeight();

	GArena->SpawnGameObject(ObjType, EObjType::UNKNOWN, CFPoint(X, Y), 0.0f, EObjectCreationFlags::CleanerEnemy);
}


void Defcon::CMilitaryMission::AddBaiter(const CFPoint& Where)
{
	TimeLastCleanerSpawned = Age;

	const float ArenaWidth = GArena->GetWidth();
	float X = (FRAND - 0.5f) * ATTACK_INITIALDISTANCE * ArenaWidth + Where.x;
	X = (float)fmod(X, ArenaWidth);
	float Y = FRANDRANGE(0.2f, 0.8f) * GArena->GetHeight();

	GArena->SpawnGameObject(EObjType::BAITER, EObjType::UNKNOWN, CFPoint(X, Y), 0.0f, EObjectCreationFlags::CleanerEnemy);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
