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

	m_pObjects = nullptr;
	m_pCarrier = nullptr;
	Age = FRAND * 10;
	RadarColor = C_MAGENTA;
	m_fSwitchTime = FRAND * 3 + 1;
	bCanBeInjured = true;
	bIsCollisionInjurious = false;
	BboxRadius.Set(4, 10);

	UE_LOG(LogGame, Log, TEXT("Creating human sprite"));
	CreateSprite(EObjType::HUMAN);
}


Defcon::CHuman::~CHuman()
{
}


bool Defcon::CHuman::IsFalling() const
{
	check(gpArena != nullptr);

	return (!this->IsBeingCarried() && Position.y > gpArena->GetTerrainElev(Position.x));
}


void Defcon::CHuman::OnAboutToDie()
{
	m_pCarrier = nullptr;

	// Tell everyone what happened.
	// If we have an abductor, he'll get told too.
	check(m_pObjects != nullptr);

	m_pObjects ->Notify(Defcon::EMessage::HumanKilled, this);
	m_pObjects2->Notify(Defcon::EMessage::HumanKilled, this);
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
			check(m_pCarrier == nullptr);
			m_pCarrier = (IGameObject*)pObj;
			check(m_pCarrier != nullptr);
			
			// Tell everyone what happened.
			m_pObjects ->Notify(Defcon::EMessage::HumanTakenAboard, this);
			m_pObjects2->Notify(Defcon::EMessage::HumanTakenAboard, this);
		}
			break;

		case Defcon::EMessage::CarrierKilled:
		case Defcon::EMessage::Released:
			check(m_pCarrier != nullptr);
			m_pCarrier = nullptr;
			break;
	}

	ILiveGameObject::Notify(msg, pObj);
}


void Defcon::CHuman::Move(float fElapsedTime)
{
	// Humans walk around mostly horizontally.
	Age += fElapsedTime;

	if(this->IsBeingCarried())
	{
		// Stay underneath the abductor.
		IGameObject* pCarrier = this->GetCarrier();
		// todo: this check failed after player died. We need to ensure that carrier is nulled out
		check(pCarrier->GetType() == EObjType::LANDER || pCarrier->GetType() == EObjType::PLAYER);
		Position = pCarrier->Position;
		Position.y -= 27.0f;

		// Init momentum for possible fall.
		Orientation.Fwd.y = -2.0f;
	}
	else
	{
		float h = gpArena->GetTerrainElev(Position.x);

		if(Position.y >= h)
		{
			// We're above the ground, so we must be 
			// falling from a killed abductor.
			Orientation.Fwd.Set(
				0.0f, 
				Orientation.Fwd.y +  Orientation.Fwd.y * 1.5f * fElapsedTime);
				
			Position.MulAdd(Orientation.Fwd, fElapsedTime);

			if(Position.y < h)
			{
				// We fall to the ground.
				if(Orientation.Fwd.y < -HUMAN_TERMINALVELOCITY)
				{
					// We landed too hard, so we're toast.
					gpArena->ExplodeObject(this);
				}
				else
				{
					// We landed okay. Give the player 250.
					gpArena->IncreaseScore((int32)HUMAN_VALUE_LIBERATED, true, &Position);
				}
			}
		}
		else
		{
			// Casually roam the terrain.
			// todo: we seem to have a 'bug' where the 
			// person likes to ascend hills.
			float f = (float)fmod(Age, 10.0f) / 10.0f;
			f = (float)sin(f * TWO_PI);
			float fy = (float)cos(f * TWO_PI);
			CFPoint motion(f+(FRAND-0.5f), fy + (FRAND - 0.5f));
			//motion.Mul(CFPoint(1.0f, 0.75f));
			motion.Mul(fElapsedTime * 10.0f);

			Position += motion;
			Position.y = FMath::Min(Position.y, h-5);
			Position.y = FMath::Max(Position.y, 20);
		}
	}
	// todo: switch to a different atlas cel every now and then
	// or based on our orientation.
	check(m_fSwitchTime != 0.0f);
	float f = (float)fmod(Age, m_fSwitchTime) / m_fSwitchTime;
	f = (float)cos(f * PI) + 1.0f;
	f /= 2;
	//Sprite->SetCurrentCel(ROUND(f));
	Sprite->FlipHorizontal = (ROUND(f) == 0);
}


void Defcon::CHuman::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}


int Defcon::CHuman::GetExplosionColorBase() const
{
	return CGameColors::red;
}


float Defcon::CHuman::GetExplosionMass() const
{
	return 0.5f;
}
