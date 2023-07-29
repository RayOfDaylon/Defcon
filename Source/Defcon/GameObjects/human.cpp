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
	m_parentType = m_type;
	m_type = ObjType::HUMAN;

	m_pObjects = nullptr;
	m_pCarrier = nullptr;
	m_fAge = FRAND * 10;
	m_smallColor = C_MAGENTA;
	m_fSwitchTime = FRAND * 3 + 1;
	m_bCanBeInjured = true;
	m_bIsCollisionInjurious = false;
	m_bboxrad.set(4, 10);

	UE_LOG(LogGame, Log, TEXT("Creating human sprite"));
	CreateSprite(ObjType::HUMAN);
}


Defcon::CHuman::~CHuman()
{
}


bool Defcon::CHuman::IsFalling() const
{
	check(gpArena != nullptr);

	return (!this->IsBeingCarried() && m_pos.y > gpArena->GetTerrainElev(m_pos.x));
}


void Defcon::CHuman::OnAboutToDie()
{
	m_pCarrier = nullptr;

	// Tell everyone what happened.
	// If we have an abductor, he'll get told too.
	check(m_pObjects != nullptr);

	m_pObjects ->Notify(Defcon::Message::human_killed, this);
	m_pObjects2->Notify(Defcon::Message::human_killed, this);
}


#ifdef _DEBUG
const char* Defcon::CHuman::GetClassname() const
{
	static const char* psz = "Human";
	return psz;
}
#endif


void Defcon::CHuman::Notify(Defcon::Message msg, void* pObj)
{
	switch(msg)
	{
		case Defcon::Message::takenaboard:
		{
			check(m_pCarrier == nullptr);
			m_pCarrier = (IGameObject*)pObj;
			check(m_pCarrier != nullptr);
			
			// Tell everyone what happened.
			m_pObjects ->Notify(Defcon::Message::human_takenaboard, this);
			m_pObjects2->Notify(Defcon::Message::human_takenaboard, this);
		}
			break;

		case Defcon::Message::carrier_killed:
		case Defcon::Message::released:
			check(m_pCarrier != nullptr);
			m_pCarrier = nullptr;
			break;
	}

	ILiveGameObject::Notify(msg, pObj);
}


void Defcon::CHuman::Move(float fElapsedTime)
{
	// Humans walk around mostly horizontally.
	m_fAge += fElapsedTime;

	if(this->IsBeingCarried())
	{
		// Stay underneath the abductor.
		IGameObject* pCarrier = this->GetCarrier();
		// todo: this check failed after player died. We need to ensure that carrier is nulled out
		check(pCarrier->GetType() == ObjType::LANDER || pCarrier->GetType() == ObjType::PLAYER);
		m_pos = pCarrier->m_pos;
		m_pos.y -= 27.0f;

		// Init momentum for possible fall.
		m_orient.fwd.y = -2.0f;
	}
	else
	{
		float h = gpArena->GetTerrainElev(m_pos.x);

		if(m_pos.y >= h)
		{
			// We're above the ground, so we must be 
			// falling from a killed abductor.
			m_orient.fwd.set(
				0.0f, 
				m_orient.fwd.y +  m_orient.fwd.y * 1.5f * fElapsedTime);
				
			m_pos.muladd(m_orient.fwd, fElapsedTime);

			if(m_pos.y < h)
			{
				// We fall to the ground.
				if(m_orient.fwd.y < -HUMAN_TERMINALVELOCITY)
				{
					// We landed too hard, so we're toast.
					gpArena->ExplodeObject(this);
				}
				else
				{
					// We landed okay. Give the player 250.
					gpArena->IncreaseScore((int32)HUMAN_VALUE_LIBERATED, true, &m_pos);
				}
			}
		}
		else
		{
			// Casually roam the terrain.
			// todo: we seem to have a 'bug' where the 
			// person likes to ascend hills.
			float f = (float)fmod(m_fAge, 10.0f) / 10.0f;
			f = (float)sin(f * TWO_PI);
			float fy = (float)cos(f * TWO_PI);
			CFPoint motion(f+(FRAND-0.5f), fy + (FRAND - 0.5f));
			//motion.mul(CFPoint(1.0f, 0.75f));
			motion.mul(fElapsedTime * 10.0f);

			m_pos += motion;
			m_pos.y = FMath::Min(m_pos.y, h-5);
			m_pos.y = FMath::Max(m_pos.y, 20);
		}
	}
	// todo: switch to a different atlas cel every now and then
	// or based on our orientation.
	check(m_fSwitchTime != 0.0f);
	float f = (float)fmod(m_fAge, m_fSwitchTime) / m_fSwitchTime;
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
