// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconGameInstance.h"
#include "Arenas/DefconPlayViewBase.h"
#include "Arenas/DefconPostwaveViewBase.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "GameObjects/human.h"
#include "Main/task.h"
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


UDefconGameInstance* GDefconGameInstance = nullptr;


void UDefconGameInstance::Init()
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);
	Super::Init();

	GDefconGameInstance = this;

	Defcon::GObjectTypeManager.Init();

	GPrefs.Init(FString(TEXT("prefs.txt")));

	GAudio = new Defcon::CAudioManager(this);


#define ADD_AUDIO_TRACK(_track, _asset)	\
	GAudio->AddTrack(_track, _asset);	\
	GAudio->OutputSound(_track, 0.01f); // Force sound to preload.

	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Focus_changed,         FocusChangedSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Invalid_selection,     InvalidSelectionSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Mission_chosen,        MissionChosenSound);

	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Wave_start,            WaveStartSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_materialize,      MaterializationSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Laserfire,             LaserSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding,        Explosion1Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding2,       Explosion2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding2a,      Explosion2ASound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding2b,      Explosion2BSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding_medium, ExplosionMediumSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding_small,  ExplosionSmallSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ship_exploding_small2, ExplosionSmall2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Playership_thrust,     ThrustSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Gulp,                  GulpSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Warp,                  WarpSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Phred,                 PhredSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::BigRed,                BigRedSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Munchie,               MunchieSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Ghostflight,           GhostFlightSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Smartbomb,             SmartbombSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Wave_end,              WaveEndSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Player_dying,          PlayerDyingSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Shieldbonk,            ShieldBonkSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Bullet,                BulletSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Bullet2,               Bullet2Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Bullet3,               Bullet3Sound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Human_abducted,        AbductionSound);
	ADD_AUDIO_TRACK(Defcon::EAudioTrack::Swarmer,               SwarmerSound);

#undef ADD_AUDIO_TRACK


	// Load up the GameObjectResources global.

	const auto UpscaleFactor = 4.25f / 3.0f;


	HumanAtlas     ->Atlas.InitCache();
	PlayerShipAtlas->Atlas.InitCache();

	GGameObjectResources.Add(Defcon::EObjType::HUMAN,    { HumanAtlas,      HumanAtlas      ->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f });
	GGameObjectResources.Add(Defcon::EObjType::PLAYER,   { PlayerShipAtlas, PlayerShipAtlas ->Atlas.GetCelPixelSize() * UpscaleFactor, 0.5f } );
}


void UDefconGameInstance::Shutdown() 
{
	UE_LOG(LogGame, Log, TEXT("%S"), __FUNCTION__);

	SAFE_DELETE(Defcon::GGameMatch);
	SAFE_DELETE(GAudio);

	Super::Shutdown();
}


bool UDefconGameInstance::IsLive() const 
{
	return (GAudio != nullptr); 
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
	Details,
	Credits
	*/

	auto PC = GetFirstLocalPlayerController();

#define ADD_VIEW(_class) Views.Add(CreateWidget<UDefconViewBase>(PC, _class));

	ADD_VIEW( IntroViewClass         );
	ADD_VIEW( MainMenuViewClass      );
	ADD_VIEW( MissionPickerViewClass );
	ADD_VIEW( PrewaveViewClass       );
	ADD_VIEW( PlayViewClass          );
	ADD_VIEW( PostwaveViewClass      );
	ADD_VIEW( GameOverViewClass      );
	ADD_VIEW( HelpViewClass          );
	ADD_VIEW( DetailsViewClass       );
	ADD_VIEW( CreditsViewClass       );

#undef ADD_VIEW
}


// todo: we should just have a OnButtonPressed method that takes a button enum
#define ON_BUTTON_PRESSED(_proc)	if(CurrentView != nullptr) { CurrentView->_proc(); }

void UDefconGameInstance::OnEscPressed        () { ON_BUTTON_PRESSED( OnEscPressed        ); }
void UDefconGameInstance::OnEnterPressed      () { ON_BUTTON_PRESSED( OnEnterPressed      ); }
void UDefconGameInstance::OnSkipPressed       () { ON_BUTTON_PRESSED( OnSkipPressed       ); }
void UDefconGameInstance::OnPausePressed      () { ON_BUTTON_PRESSED( OnPausePressed      ); }
void UDefconGameInstance::OnBulletTimePressed () { ON_BUTTON_PRESSED( OnBulletTimePressed ); }

#undef ON_BUTTON_PRESSED


void UDefconGameInstance::OnShiftPressed(bool IsDown)
{
	if(CurrentView != nullptr) 
	{
		CurrentView->OnShiftPressed(IsDown);
	}
}


#define GET_VIEW	if(CurrentView == nullptr)	{ return; }


void UDefconGameInstance::OnNavEvent(ENavigationKey Key)
{
	GET_VIEW
	CurrentView->OnNavEvent(Key);
}


void UDefconGameInstance::OnPawnNavEvent(EDefconPawnNavigationEvent Event, bool Active)
{
	GET_VIEW
	CurrentView->OnPawnNavEvent(Event, Active);
}


void UDefconGameInstance::OnPawnWeaponEvent(EDefconPawnWeaponEvent Event, bool Active)
{
	// Note: not every weapon repeats while button held down, so <Active> may be moot

	GET_VIEW
	CurrentView->OnPawnWeaponEvent(Event, Active);
}


void UDefconGameInstance::OnToggleDebugStats()
{
	GET_VIEW
	CurrentView->OnToggleDebugStats();
}


void UDefconGameInstance::OnToggleShowBoundingBoxes()
{
	GET_VIEW
	CurrentView->OnToggleShowBoundingBoxes();
}


void UDefconGameInstance::OnToggleShowOrigin()
{
	GET_VIEW
	CurrentView->OnToggleShowOrigin();
}

#undef GET_VIEW


#define GET_PLAYVIEW		auto View = Cast<UDefconPlayViewBase>(CurrentView);	if(View == nullptr)	{ return; }


void UDefconGameInstance::OnToggleGodMode()
{
	GET_PLAYVIEW

	Defcon::GGameMatch->SetGodMode(!Defcon::GGameMatch->GetGodMode());

	Defcon::GMessageMediator.TellUser(FString::Printf(TEXT("God mode %s"), Defcon::GGameMatch->GetGodMode() ? TEXT("ON") : TEXT("OFF")), 0.0f, Defcon::EDisplayMessage::GodModeChanged);
}


void UDefconGameInstance::OnSelectGameObjectToSpawn()
{
	GET_PLAYVIEW

	View->OnSelectGameObjectToSpawn();
}


void UDefconGameInstance::OnSpawnGameObject()
{
	GET_PLAYVIEW

	const auto& PlayerShip = Defcon::GGameMatch->GetPlayerShip();

	const FString Str = FString::Printf(TEXT("Spawning enemy near player's position %d, %d"), (int32)PlayerShip.Position.x, (int32)PlayerShip.Position.y);
	Defcon::GMessageMediator.TellUser(Str, 0.25f);

	View->OnSpawnGameObject();
}


void UDefconGameInstance::OnIncrementXp()
{
	GET_PLAYVIEW

	Defcon::GGameMatch->AdjustScore(5000);
	//Score += 5000;

	const FString Str = FString::Printf(TEXT("XP increased to %d"), Defcon::GGameMatch->GetScore());
	Defcon::GMessageMediator.TellUser(Str, 1.0f, Defcon::EDisplayMessage::XpChanged);
}


void UDefconGameInstance::OnDecrementXp()
{
	GET_PLAYVIEW

	if(Defcon::GGameMatch->GetScore() <= 0)
	{
		return;
	}

	Defcon::GGameMatch->AdjustScore(-5000);

	const FString Str = FString::Printf(TEXT("XP decreased to %d"), Defcon::GGameMatch->GetScore());
	Defcon::GMessageMediator.TellUser(Str, 1.0f, Defcon::EDisplayMessage::XpChanged);
}

#undef GET_PLAYVIEW


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

	if(MatchInProgress() && (!View->RequiresGameMatch()))
	{
		DELETE_AND_NULL(Defcon::GGameMatch);
	}

	ViewportClient->RemoveAllViewportWidgets();

	CurrentView = View;

	CurrentView->AddToViewport();
	CurrentView->SetIsEnabled(true);
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

	SetCurrentView(ArenaWidget);
}


void UDefconGameInstance::OnMissionEnded(const FString& NameOfEndedMission, bool StateOfEndedMission, bool HumansWereInvolved)
{
	const int32 NumHumans = Defcon::GGameMatch->GetHumans().Count();

	if(Defcon::GGameMatch->GetPlayerShip().IsAlive() || NumHumans > 0)
	{
		auto PostWaveView = Cast<UDefconPostwaveViewBase>(Views[(int32)EDefconArena::Postwave]);

		PostWaveView->SetTitle(FString::Printf(TEXT("MISSION \"%s\" %s"), *NameOfEndedMission, StateOfEndedMission ? TEXT("COMPLETED") : TEXT("ABORTED")).ToUpper());

		FString Str;

		if(HumansWereInvolved)
		{
			if(NumHumans > 0)
			{
				Str = FString::Printf(TEXT("%s human%S remaining"), *GetNumberSpelling(NumHumans, 1), NumHumans > 1 ? "s" : "");
			}
			else
			{
				Str = TEXT("All humans killed or abducted");
			}
		}

		PostWaveView->SetSubtitle(Str);

		TransitionToArena(EDefconArena::Postwave);
	}
	else
	{
		TransitionToArena(EDefconArena::GameOver);
	}
}


void UDefconGameInstance::StartGameMatch(Defcon::EMissionID InMissionID)
{
	check(!MatchInProgress());

	Defcon::GGameMatch = new Defcon::CGameMatch((Defcon::EMissionID)InMissionID);

	GetStats().Reset();
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
