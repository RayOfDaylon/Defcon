// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconGameInstance.h"
#include "Arenas/DefconPlayViewBase.h"
#include "Arenas/DefconPostwaveViewBase.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "GameObjects/human.h"
#include "Main/event.h"
#include "DefconLogging.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Globals/prefs.h"
#include "Globals/_sound.h"
#include "Globals/GameObjectResources.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


UDefconGameInstance* gDefconGameInstance = nullptr;


void UDefconGameInstance::Init()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);
	Super::Init();

	gDefconGameInstance = this;

	Defcon::ObjectTypeManager.Init();

	gPrefs.Init(FString(TEXT("prefs.txt")));

	gpAudio = new Defcon::CAudioManager(this);


#define ADD_AUDIO_TRACK(_track, _asset)	\
	gpAudio->AddTrack(_track, _asset);	\
	gpAudio->OutputSound(_track, 0.01f); // Force sound to preload.

	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_focus_changed,         FocusChangedSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_invalid_selection,     InvalidSelectionSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_mission_chosen,        MissionChosenSound);

	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_wave_start,            WaveStartSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_materialize,      MaterializationSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_laserfire,             LaserSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding,        Explosion1Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding2,       Explosion2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding2a,      Explosion2ASound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding2b,      Explosion2BSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding_medium, ExplosionMediumSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding_small,  ExplosionSmallSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ship_exploding_small2, ExplosionSmall2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_playership_thrust,     ThrustSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_gulp,                  GulpSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_warp,                  WarpSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_phred,                 PhredSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_bigred,                BigRedSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_munchie,               MunchieSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_ghostflight,           GhostFlightSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_smartbomb,             SmartbombSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_wave_end,              WaveEndSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_player_dying,          PlayerDyingSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_shieldbonk,            ShieldBonkSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_bullet,                BulletSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_bullet2,               Bullet2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_bullet3,               Bullet3Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_human_abducted,        AbductionSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::snd_swarmer,               SwarmerSound);

#undef ADD_AUDIO_TRACK


	// Load up the GameObjectResources global.

	const auto UpscaleFactor = 4.25f / 3.0f;


	HumanAtlas          ->Atlas.InitCache();
	PlayerShipAtlas     ->Atlas.InitCache();

	GameObjectResources.Add(Defcon::ObjType::HUMAN,    { HumanAtlas,      HumanAtlas      ->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f });
	GameObjectResources.Add(Defcon::ObjType::PLAYER,   { PlayerShipAtlas, PlayerShipAtlas ->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f } );

	PlayerShipPtr = new Defcon::CPlayer;
}


void UDefconGameInstance::Shutdown() 
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	SAFE_DELETE(PlayerShipPtr);
	SAFE_DELETE(m_pMission);
	SAFE_DELETE(gpAudio);

	Super::Shutdown();
}


bool UDefconGameInstance::IsLive() const 
{
	return (gpAudio != nullptr); 
}


bool UDefconGameInstance::Update(float DeltaTime)
{
	// Forward the elapsed time within the mission
	// to the current mission, and if it has ended, 
	// then make the next mission and if it does 
	// not exist (no more missions), then we end.

	if(m_pMission != nullptr)
	{
		return m_pMission->Update(DeltaTime);
	}

	return (m_pMission != nullptr);
}


void UDefconGameInstance::AddEvent(Defcon::CEvent* Event)
{
	check(m_pMission != nullptr);

	m_pMission->AddEvent(Event);
}




void UDefconGameInstance::CreateArenas()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	/*
	Intro,
	MainMenu,
	MissionPicker,
	Prewave,
	Play,
	Postwave,
	GameOver,
	Help,
	Credits
	*/

	auto PC = GetFirstLocalPlayerController();

	Views.Add(CreateWidget<UDefconViewBase>(PC, IntroViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, MainMenuViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, MissionPickerViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, PrewaveViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, PlayViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, PostwaveViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, GameOverViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, HelpViewClass));
	Views.Add(CreateWidget<UDefconViewBase>(PC, CreditsViewClass));
}


void UDefconGameInstance::OnEscPressed()
{
	// Forward event to current view.

	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnEscPressed();
}


void UDefconGameInstance::OnEnterPressed()
{
	// Forward event to current view.

	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnEnterPressed();
}


void UDefconGameInstance::OnSkipPressed()
{
	// Forward event to current view.

	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnSkipPressed();
}


void UDefconGameInstance::OnNavEvent(ENavigationKey Key)
{
	// Forward event to current view.

	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnNavEvent(Key);
}


void UDefconGameInstance::OnPawnNavEvent(EDefconPawnNavigationEvent Event, bool Active)
{
	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnPawnNavEvent(Event, Active);
}


void UDefconGameInstance::OnPawnWeaponEvent(EDefconPawnWeaponEvent Event, bool Active)
{
	// Note: not every weapon repeats while button held down, so <Active> may be moot

	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnPawnWeaponEvent(Event, Active);
}


void UDefconGameInstance::OnToggleDebugStats()
{
	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnToggleDebugStats();
}


void UDefconGameInstance::OnToggleShowBoundingBoxes()
{
	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnToggleShowBoundingBoxes();
}


void UDefconGameInstance::OnToggleShowOrigin()
{
	if(CurrentView == nullptr)
	{
		return;
	}

	CurrentView->OnToggleShowOrigin();
}


void UDefconGameInstance::OnToggleGodMode()
{
	GodMode = !GodMode;

	auto View = Cast<UDefconPlayViewBase>(CurrentView);
	
	if(View == nullptr)
	{
		return;
	}

	const FString Str = FString::Printf(TEXT("God mode %s"), GodMode ? TEXT("ON") : TEXT("OFF"));

	View->AddMessage(Str);
}


void UDefconGameInstance::OnSpawnEnemy()
{
	auto View = Cast<UDefconPlayViewBase>(CurrentView);
	
	if(View == nullptr)
	{
		return;
	}

	
	const FString Str = FString::Printf(TEXT("Spawning lander at player's position %d, %d"), (int32)PlayerShipPtr->m_pos.x, (int32)PlayerShipPtr->m_pos.y);

	View->AddMessage(Str, 0.5f);

	View->OnSpawnEnemy();
}


void UDefconGameInstance::OnIncrementXp()
{
	auto View = Cast<UDefconPlayViewBase>(CurrentView);
	
	if(View == nullptr)
	{
		return;
	}

	Score += 5000;

	const FString Str = FString::Printf(TEXT("XP increased to %d"), Score);

	View->AddMessage(Str, 1.0f);
}


void UDefconGameInstance::OnDecrementXp()
{
	auto View = Cast<UDefconPlayViewBase>(CurrentView);
	
	if(View == nullptr)
	{
		return;
	}

	if(Score <= 0)
	{
		return;
	}

	Score = FMath::Max(0, Score - 5000);

	const FString Str = FString::Printf(TEXT("XP decreased to %d"), Score);

	View->AddMessage(Str, 1.0f);
}


void UDefconGameInstance::SetCurrentView(UDefconViewBase* View)
{
	auto ViewportClient = GetGameViewportClient();

	if(!IsValid(ViewportClient))
	{
		UE_LOG(LogGame, Error, TEXT("%S: no viewport client"), __FUNCTION__);
		return;
	}

	if(CurrentView != nullptr)
	{
		CurrentView->SetIsEnabled(false);
		CurrentView->OnDeactivate();
	}

	ViewportClient->RemoveAllViewportWidgets();

	CurrentView = View;
	CurrentView->AddToViewport();

	CurrentView->SetIsEnabled(true);
	//UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetFirstLocalPlayerController(), CurrentView, EMouseLockMode::DoNotLock, true);

	CurrentView->OnActivate();
}


void UDefconGameInstance::TransitionToArena(EDefconArena Arena)
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	// Close the arena we're in, and load the arena requested.
	// todo: signal an outro to the current arena and schedule the new arena loading for later with a delegate.
	// todo: parent all our view bp's to a UDefconArena parent.


	if((int32)Arena >= Views.Num())
	{
		UE_LOG(LogGame, Error, TEXT("%S: unknown arena type %d"), __FUNCTION__, (int32)Arena);
		return;
	}

	UDefconViewBase* ArenaWidget = Views[(int32)Arena];

	if(!IsValid(ArenaWidget))
	{
		UE_LOG(LogGame, Error, TEXT("%S: invalid or garbage collected view widget for arena %d"), __FUNCTION__, (int32)Arena);
		return;
	}

	UE_LOG(LogGame, Log, TEXT("%S: transitioning to arena %d"), __FUNCTION__, (int32)Arena);

	if(Arena == EDefconArena::MissionPicker)
	{
		// Start new game with initial number of items.

		SmartbombsLeft = SMARTBOMB_INITIAL;

		Humans.DeleteAll();

		for(int32 i = 0; i < HUMANS_COUNT; i++)
		{
			Humans.Add(new Defcon::CHuman);
		}
	}

	SetCurrentView(ArenaWidget);
}


void UDefconGameInstance::InitMission(UDefconPlayViewBase* p)
{
	check(m_pMission);

	m_pMission->Init(p);
}


void UDefconGameInstance::MissionEnded()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	const FString NameOfEndedMission  = GetCurrentMissionName();
	const bool    StateOfEndedMission = GetMission()->IsCompleted();
	const bool    HumansWereInvolved  = GetMission()->HumansInvolved();

	m_pMission->Conclude();

	Defcon::IMission* p = nullptr;
	
	// Only progress to the next mission if one or more humans survived.
	if(GetHumans().Count() > 0)
	{
		p = Defcon::CMissionFactory::MakeNext(m_pMission);
	}
	
	SAFE_DELETE(m_pMission);

	m_pMission = p;

	MissionID = (m_pMission != nullptr ? m_pMission->GetID() : Defcon::MissionID::notdef);

	auto PostWaveView = Cast<UDefconPostwaveViewBase>(Views[(int32)EDefconArena::Postwave]);

	PostWaveView->SetTitle(FString::Printf(TEXT("MISSION \"%s\" %s"), *NameOfEndedMission, StateOfEndedMission ? TEXT("COMPLETED") : TEXT("ABORTED")).ToUpper());

	FString Str;

	if(HumansWereInvolved)
	{
		int32 n = GetHumans().Count();

		if(n > 0)
		{
			Str = FString::Printf(TEXT("%s human%S remaining"), *GetNumberSpelling(n, 1), n > 1 ? "s" : "");
		}
		else
		{
			Str = TEXT("All humans killed or abducted");
		}
	}

	PostWaveView->SetSubtitle(Str);

	TransitionToArena(EDefconArena::Postwave);
}


int32 UDefconGameInstance::AdvanceScore(int32 Amount)
{
	const int32 oldsmart  = Score / SMARTBOMB_VALUE;

	Score += Amount; 

#if 0
	const int32 oldplayer = Score / PLAYER_REWARD_POINTS;
	if(Score / PLAYER_REWARD_POINTS > oldplayer)
	{
		//m_nPlayerLivesLeft++;
		gpAudio->StopPlayingSound();
		gpAudio->OutputSound(CAudioManager::snd_extra_ship);
	}
#endif

	if(Score / SMARTBOMB_VALUE > oldsmart)
	{
		SmartbombsLeft += SMARTBOMB_RESUPPLY;
	}

	return Score; 
}


bool UDefconGameInstance::AcquireSmartBomb()
{
	if(SmartbombsLeft > 0)
	{
		SmartbombsLeft--;
		return true;
	}

	return false;
}


FString UDefconGameInstance::GetCurrentMissionName() const
{
	if(m_pMission == nullptr)
	{
		return TEXT("");
	}

	return m_pMission->GetName();
}


void UDefconGameInstance::SetCurrentMission(Defcon::MissionID InMissionID)
{
	SAFE_DELETE(m_pMission);

	MissionID = InMissionID;

	m_pMission = Defcon::CMissionFactory::Make(InMissionID);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
