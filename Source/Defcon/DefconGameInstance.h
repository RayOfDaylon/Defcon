// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefconViewBase.h"
#include "Arenas/DefconArena.h"
#include "Missions/mission.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "GameObjects/player.h"
#include "GameObjects/gameobj.h"
#include "GameObjects/GameObjectCollection.h"
#include "GameObjects/obj_types.h"
#include "DefconGameInstance.generated.h"




/*
	Holds data that exists across missions, like player score, smartbomb count, humans, etc.
	Also manages transitions between missions and views (arenas).
*/
UCLASS()
class DEFCON_API UDefconGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	// UGameInstance
	virtual void Init     () override;
	virtual void Shutdown () override;


	public:
	UFUNCTION(BlueprintCallable, Category="Defcon")
	void CreateArenas();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void TransitionToArena(EDefconArena Arena);

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnEscPressed();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnEnterPressed();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnSkipPressed();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnNavEvent(ENavigationKey Key);

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnPawnNavEvent(EDefconPawnNavigationEvent Event, bool Active);

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnPawnWeaponEvent(EDefconPawnWeaponEvent Event, bool Active);

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnToggleDebugStats();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnToggleShowBoundingBoxes();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnToggleShowOrigin();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnToggleGodMode();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnSpawnEnemy();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnIncrementXp();

	UFUNCTION(BlueprintCallable, Category="Defcon")
	void OnDecrementXp();

	// -------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> IntroViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> MainMenuViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> MissionPickerViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> PrewaveViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> PlayViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> PostwaveViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> GameOverViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> HelpViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Views")
	TSubclassOf<UDefconViewBase> CreditsViewClass;

	// -- Audio ----------------------------------------------------------


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> FocusChangedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> InvalidSelectionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> MissionChosenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> WaveStartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> LaserSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Explosion1Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Explosion2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Explosion2ASound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Explosion2BSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> ExplosionSmallSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> ExplosionSmall2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> ExplosionMediumSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> ThrustSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> GulpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> WarpSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> PhredSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> BigRedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> MunchieSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> SwarmerSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> GhostFlightSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> SmartbombSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> WaveEndSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> PlayerDyingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> ShieldBonkSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> BulletSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Bullet2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> Bullet3Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> AbductionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	TObjectPtr<USoundBase> MaterializationSound;
	

	// -- Common textures ------------------------------------------------

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> HumanAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> PlayerShipAtlas;




	Defcon::IMission* m_pMission = nullptr;

	Defcon::EMissionID MissionID = Defcon::EMissionID::First;

	UDefconPlayViewBase* GetPlayView() { return Cast<UDefconPlayViewBase>(Views[(int32)EDefconArena::Play]); }

	UDefconViewBase* GetCurrentView      () const { return CurrentView; }
	void             SetCurrentView      (UDefconViewBase* View);
	void             SetCurrentMission   (Defcon::EMissionID InMissionID);

	protected:

	UPROPERTY(Transient)
	TArray<UDefconViewBase*> Views;

	UDefconViewBase* CurrentView = nullptr;

	// Gameplay objects that persist across missions.
	Defcon::CPlayer*               PlayerShipPtr;
	Defcon::CGameObjectCollection  Humans;


	// The score isn't visually shown, it's used more to track XP.
	int32 Score = 0;
	Daylon::TBindableValue<int32> SmartbombsLeft;
	bool GodMode = false;


	public:

	bool                                 IsLive                 () const;
	bool                                 GetGodMode             () const { return GodMode; }
	bool                                 Update                 (float DeltaTime);
	int32                                GetScore               () const { return Score; }
	void                                 SetScore               (int32 Amount) { Score = Amount; }
	int32                                AdvanceScore           (int32 Amount);
	Defcon::CPlayer&                     GetPlayerShip          () { check(PlayerShipPtr != nullptr); return *PlayerShipPtr; }
	const Defcon::CGameObjectCollection& GetConstHumans         () const { return Humans; }
	Defcon::CGameObjectCollection&       GetHumans              () { return Humans; }
	bool                                 AcquireSmartBomb       ();
	int32                                GetSmartbombCount      () const { return SmartbombsLeft; }
	void                                 BindToSmartbombCount   (TFunction<void(const int32& Val)> Delegate) { SmartbombsLeft.Bind(Delegate); }
	void                                 HostileDestroyed       (Defcon::EObjType Kind) { if(m_pMission != nullptr) ((Defcon::CMilitaryMission*)m_pMission)->HostileDestroyed(Kind); }
	Defcon::IMission*                    GetMission             () { return m_pMission; }
	void                                 InitMission            (UDefconPlayViewBase* p);
	void                                 MissionEnded           ();
	FString                              GetCurrentMissionName  () const;
	void                                 AddEvent               (Defcon::CEvent* Event);
};

extern UDefconGameInstance* gDefconGameInstance;
