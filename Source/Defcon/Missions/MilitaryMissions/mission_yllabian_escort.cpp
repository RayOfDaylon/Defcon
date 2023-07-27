/*
	mission_yllabian_escort.cpp
	"Yllabian Dogfight 2" but in orbit.

	Copyright 2010 Daylon Graphics Ltd.
*/



#include "MilitaryMission.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


#include "Common/util_color.h"
#include "Common/util_geom.h"


#include "Globals/prefs.h"

#include "GameObjects/player.h"
#include "GameObjects/obj_types.h"

#include "Arenas/DefconPlayViewBase.h"




// ----------------------------------------------------------


void Defcon::CYllabianEscort::Init(UDefconPlayViewBase* pA)
{
	CMilitaryMission::Init(pA);

	m_nHostilesRemaining = 24+30+30+30
							+3+3+3+3
							+3+3+3
							+10+7+4+3
							+3+4+4+3
							+10+12+10+5;

	m_nLandersRemaining =  10 + 12 + 10 + 5;
}


bool Defcon::CYllabianEscort::Update(float fElapsed)
{
	if(!CMilitaryMission::Update(fElapsed))
		return false;

	this->DoIntroText(fElapsed);

	return true;
}


void Defcon::CYllabianEscort::DoIntroText(float fElapsed)
{
	// If we already created the intro text, then do nothing.
	if(m_bIntroDone)
		return;

	m_bIntroDone = true;
	// First time here; create intro text objects.


	const char* psz[] = 
	{
		"Yllabian fighters are escorting a lander attack.",
		"Engage enemy forces and eliminate them."
	};

	for(auto Text : psz)
	{
		m_pArena->AddMessage(Text);
	}
}


void Defcon::CYllabianEscort::MakeTargets(float fElapsed, const CFPoint& where)
{
	if(this->HostilesRemaining() > 0 
		&& m_fAge >= 
			DELAY_BEFORE_ATTACK + 
			(DELAY_BETWEEN_REATTACK + 5) * 3.5
			)
	{
		this->AddMissionCleaner(where);
	}


	if((this->HostilesInPlay() == 0 && m_fRepopCounter > DELAY_BEFORE_ATTACK) 
		|| (this->HostilesInPlay() > 0 && m_fRepopCounter > DELAY_BETWEEN_REATTACK))
	{
		m_fRepopCounter = 0.0f;

		if(m_nAttackWave >= 4)
			return;

		const Wave waves[] = 
		{
			{ ObjType::GUPPY,   { 24, 30, 30, 30 } },
			{ ObjType::SWARMER, {  3,  3,  3,  3 } },
			{ ObjType::POD,     {  3,  3,  3,  0 } },
			{ ObjType::DYNAMO,  { 10,  7,  4,  3 } },
			{ ObjType::HUNTER,  {  3,  4,  4,  3 } },
			{ ObjType::LANDER,  { 10, 12, 10,  5 } }
		};


		size_t i, j;
		for(i = 0; i < array_size(waves); i++)
		{
			for(j = 0; j < waves[i].count[m_nAttackWave] && this->HostilesRemaining() > 0; j++)
			{
				float wp = m_pArena->GetWidth();
				CFPoint P;

				switch(waves[i].type)
				{
					case ObjType::LANDER:
						P.x = FRAND * (m_pArena->GetWidth() - 1);
						P.y = FRANDRANGE(0.85f, 1.0f);
						break;

					case ObjType::POD:
					case ObjType::HUNTER:
					case ObjType::DYNAMO:
						P.x = FRAND * (m_pArena->GetWidth() - 1);
						P.y = FRANDRANGE(0.15f, 0.85f);
						break;

					default:
						P.x = (FRAND - 0.5f) * m_pArena->GetDisplayWidth() + wp/2;
						P.y = FRANDRANGE(0.25f, 0.75f);
						break;
				}
				P.x = (float)fmod(P.x, wp);
				P.y *= m_pArena->GetHeight();

				m_pArena->CreateEnemy(waves[i].type, P, FRANDRANGE(0.0f, 0.1f * j), true, true);
			}
		}

		m_nAttackWave++;
	}
}
