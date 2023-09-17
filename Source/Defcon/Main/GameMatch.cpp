#include "GameMatch.h"
#include "GameObjects/human.h"
#include "Globals/prefs.h"
#include "DefconGameInstance.h"


extern Defcon::CGameMatch* Defcon::GGameMatch = nullptr;


Defcon::CGameMatch::CGameMatch(EMissionID InMissionID)
{
	GGameMatch = this;

	PlayerShip = new Defcon::CPlayerShip;

	Humans.DeleteAll();

	for(int32 Idx = 0; Idx < HUMANS_COUNT; Idx++)
	{
		Humans.Add(new CHuman);
		Humans.GetFirst()->SetID(Idx);
	}

	bHumansPlaced = false;

	Score = 0;

	SetCurrentMission((Defcon::EMissionID)InMissionID);
}


Defcon::CGameMatch::~CGameMatch()
{
	SAFE_DELETE(PlayerShip);
	SAFE_DELETE(Mission);

	GGameMatch = nullptr;
}


void Defcon::CGameMatch::AdjustScore(int32 Amount)
{
	Score = FMath::Max(0, Score + Amount);
}


int32 Defcon::CGameMatch::AdvanceScore(int32 Amount)
{
	check(Amount >= 0);

	// todo: have smartbombs be earned by powerup
	const int32 OldSmart = Score / SMARTBOMB_VALUE;

	Score += Amount; 

	if(Score / SMARTBOMB_VALUE > OldSmart)
	{
		PlayerShip->AddSmartBombs(SMARTBOMB_RESUPPLY);
	}

	return Score; 
}


bool Defcon::CGameMatch::Update(float DeltaTime)
{
	// Forward the elapsed time within the mission
	// to the current mission, and if it has ended, 
	// then make the next mission and if it does 
	// not exist (no more missions), then we end.

	if(Mission != nullptr)
	{
		return Mission->Update(DeltaTime);
	}

	return false;
}


void Defcon::CGameMatch::InitMission()
{
	check(Mission);

	Mission->Init();
}


void Defcon::CGameMatch::MissionEnded()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	const FString NameOfEndedMission  = GetCurrentMissionName();
	const bool    StateOfEndedMission = GetMission()->IsMissionComplete();
	const bool    HumansWereInvolved  = GetMission()->HumansInvolved();

	Mission->Conclude();

	Defcon::IMission* NextMission = nullptr;
	
	// Only progress to the next mission if one or more humans survived.
	if(Defcon::GGameMatch->GetHumans().Count() > 0)
	{
		NextMission = Defcon::CMissionFactory::MakeNext(Mission);
	}
	
	SAFE_DELETE(Mission);

	Mission = NextMission;

	MissionID = (Mission != nullptr ? Mission->GetID() : Defcon::EMissionID::Undefined);

	// todo: could use a delegate here to avoid depending on game instance.
	GDefconGameInstance->OnMissionEnded(NameOfEndedMission, StateOfEndedMission, HumansWereInvolved);
}


FString Defcon::CGameMatch::GetCurrentMissionName() const
{
	if(Mission == nullptr)
	{
		return TEXT("");
	}

	return Mission->GetName();
}


void Defcon::CGameMatch::SetCurrentMission(Defcon::EMissionID InMissionID)
{
	SAFE_DELETE(Mission);

	MissionID = InMissionID;

	Mission = Defcon::CMissionFactory::Make(InMissionID);

	PlayerShip->SetShieldStrength(1.0f);
}
