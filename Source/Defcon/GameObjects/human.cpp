// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/* 
	human.cpp
	Implements human entities in Defcon game.
*/


#include "human.h"

#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/Enemies/enemies.h"
#include "GameObjects/player.h"
#include "Arenas/DefconPlayViewBase.h"
#include "DefconLogging.h"



Defcon::CHuman::CHuman()
{
	ParentType = Type;
	Type = EObjType::HUMAN;

	Objects = nullptr;
	Carrier = nullptr;
	Age = FRAND * 10;
	RadarColor = C_MAGENTA;
	SwitchFacingDirectionCountdown = FRANDRANGE(1.0f, 4.0f);
	bCanBeInjured = true;
	bIsCollisionInjurious = false;
	BboxRadius.Set(4, 10);
	
	WalkingSpeed = FRANDRANGE(8.0f, 12.0f);
	SwitchWalkingDirectionCountdown = FRANDRANGE(2.0f, 5.0f);

	const auto V = Daylon::RandVector2D();
	Motion.Set(V.X, V.Y * 0.25f);
	Motion.Normalize();

	//UE_LOG(LogGame, Log, TEXT("Creating human sprite"));
	CreateSprite(EObjType::HUMAN);
}


Defcon::CHuman::~CHuman()
{
}


bool Defcon::CHuman::IsFalling() const
{
	check(GArena != nullptr);

	return (!IsBeingCarried() && Position.y > GArena->GetTerrainElev(Position.x));
}


void Defcon::CHuman::OnAboutToDie()
{
	Carrier = nullptr;

	// Tell everyone what happened.
	// If we have an abductor, he'll get told too.
	check(Objects != nullptr);

	Objects ->Notify(Defcon::EMessage::HumanKilled, this);
	Objects2->Notify(Defcon::EMessage::HumanKilled, this);
}


#ifdef _DEBUG
const char* Defcon::CHuman::GetClassname() const
{
	static const char* psz = "Human";
	return psz;
}
#endif


void Defcon::CHuman::Notify(Defcon::EMessage msg, void* pObj)
{
	switch(msg)
	{
		case Defcon::EMessage::TakenAboard:
		{
			check(Carrier == nullptr);
			Carrier = (IGameObject*)pObj;
			check(Carrier != nullptr);
			
			// Tell everyone what happened.
			Objects ->Notify(Defcon::EMessage::HumanTakenAboard, this);
			Objects2->Notify(Defcon::EMessage::HumanTakenAboard, this);
		}
			break;

		case Defcon::EMessage::CarrierKilled:
		case Defcon::EMessage::Released:
			check(Carrier != nullptr);
			Carrier = nullptr;
			break;
	}

	ILiveGameObject::Notify(msg, pObj);
}


void Defcon::CHuman::Move(float DeltaTime)
{
	// Humans walk around mostly horizontally.
	Age += DeltaTime;


	// Flip sprite every now and then.
	SwitchFacingDirectionCountdown -= DeltaTime;

	if(SwitchFacingDirectionCountdown <= 0.0f)
	{
		// Make player "thrash frantically" if being abducted.
		if(IsBeingCarried() && GetCarrier()->GetType() != EObjType::PLAYER)
		{
			SwitchFacingDirectionCountdown = FRANDRANGE(0.25f, 1.0f);
		}
		else
		{
			SwitchFacingDirectionCountdown = FRANDRANGE(1.0f, 4.0f);
		}
	
		Sprite->FlipHorizontal = !Sprite->FlipHorizontal;
	}


	if(IsBeingCarried())
	{
		// Stay underneath the abductor.
		IGameObject* pCarrier = GetCarrier();
		// todo: this check failed after player died. We need to ensure that carrier is nulled out
		check(pCarrier->GetType() == EObjType::LANDER || pCarrier->GetType() == EObjType::PLAYER);
		Position = pCarrier->Position;
		Position.y -= 27.0f;

		// Init momentum for possible fall.
		Orientation.Fwd.y = -2.0f;
	}
	else
	{
		float MaxH = GArena->GetTerrainElev(Position.x);

		if(Position.y >= MaxH)
		{
			// We're above the ground, so we must be 
			// falling from a killed abductor.
			Orientation.Fwd.Set(
				0.0f, 
				Orientation.Fwd.y +  Orientation.Fwd.y * 1.5f * DeltaTime);
				
			Position.MulAdd(Orientation.Fwd, DeltaTime);

			if(Position.y < MaxH)
			{
				// We've fallen to the ground.
				if(Orientation.Fwd.y < -HUMAN_TERMINALVELOCITY)
				{
					// We landed too hard, so we're toast.
					GArena->DestroyObject(this);
				}
				else
				{
					// We landed okay.
					GArena->IncreaseScore(HUMAN_VALUE_LIBERATED, true, &Position);
				}
			}
		}
		else
		{
			// Casually roam the terrain.

			SwitchWalkingDirectionCountdown -= DeltaTime;

			if(SwitchWalkingDirectionCountdown <= 0.0f)
			{
				SwitchWalkingDirectionCountdown = FRANDRANGE(2.0f, 5.0f);

				const auto V = Daylon::RandVector2D();
				Motion.Set(V.X, V.Y * 0.25f);
				Motion.Normalize();

				// Every now and then, have the human just loiter.
				WalkingSpeed = FRAND < 0.25f ? 0.1f : FRANDRANGE(8.0f, 12.0f);
			}

			Position.MulAdd(Motion, DeltaTime * WalkingSpeed);

			MaxH = GArena->GetTerrainElev(GArena->WrapX(Position.x));

			Position.y = FMath::Min(Position.y, MaxH - 5);
			Position.y = FMath::Max(Position.y, 20);
		}
	}
}


Defcon::EColor Defcon::CHuman::GetExplosionColorBase() const
{
	return EColor::Red;
}


float Defcon::CHuman::GetExplosionMass() const
{
	return 0.5f;
}
