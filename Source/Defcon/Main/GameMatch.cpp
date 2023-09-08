#include "GameMatch.h"
#include "GameObjects/human.h"
#include "Globals/prefs.h"

extern Defcon::CGameMatch* Defcon::GGameMatch = nullptr;


Defcon::CGameMatch::CGameMatch(EMissionID InMissionID)
{
	GGameMatch = this;

	PlayerShipPtr = new Defcon::CPlayerShip;

	Humans.DeleteAll();

	for(int32 i = 0; i < HUMANS_COUNT; i++)
	{
		Humans.Add(new CHuman);
	}

	bHumansPlaced = false;

	SmartbombsLeft = SMARTBOMB_INITIAL;

	Score = 0;
}


Defcon::CGameMatch::~CGameMatch()
{
	SAFE_DELETE(PlayerShipPtr);

	GGameMatch = nullptr;
}


bool Defcon::CGameMatch::AcquireSmartBomb()
{
	if(SmartbombsLeft > 0)
	{
		SmartbombsLeft--;
		return true;
	}

	return false;
}


void Defcon::CGameMatch::AdjustScore(int32 Amount)
{
	Score = FMath::Max(0, Score + Amount);
}


int32 Defcon::CGameMatch::AdvanceScore(int32 Amount)
{
	const int32 oldsmart  = Score / SMARTBOMB_VALUE;

	Score += Amount; 

#if 0
	const int32 oldplayer = Score / PLAYER_REWARD_POINTS;
	if(Score / PLAYER_REWARD_POINTS > oldplayer)
	{
		//m_nPlayerLivesLeft++;
		GAudio->StopPlayingSound();
		GAudio->OutputSound(CAudioManager::extra_ship);
	}
#endif

	if(Score / SMARTBOMB_VALUE > oldsmart)
	{
		SmartbombsLeft += SMARTBOMB_RESUPPLY;
	}

	return Score; 
}
