// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "DefconViewBase.h"
#include "Widgets/DefconPlayMainWidgetBase.h"
#include "Widgets/DefconPlayStatsWidgetBase.h"
#include "Widgets/DefconPlayRadarWidgetBase.h"
#include "GameObjects/terrain.h"
#include "GameObjects/Auxiliary/materialization.h"
#include "GameObjects/GameObjectCollection.h"
#include "Main/task.h"
#include "DefconPlayViewBase.generated.h"



namespace Defcon
{
	class IBullet;
	class CHuman;
}


/*
	Base class of the play view widget blueprint, which holds the main screen, the radar screen, 
	and the stats panel, i.e.:

	 -------------------------------------------------
	|          |                                      |
	|  Stats   |              Radar                   |
	|__________|______________________________________|
	|                                                 |
	|                                                 |
	|                                                 |
	|           Main                                  |
	|                                                 |
	|                                                 |
	|_________________________________________________|

	Members have lifetime scope limited to this view being current.
	Objects like the player and humans that persist across missions are in the game instance.
*/
UCLASS()
class DEFCON_API UDefconPlayViewBase : public UDefconViewBase
{
	GENERATED_BODY()

	friend class Defcon::CEndMissionTask;

	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	/*virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;*/


	//virtual void OnActivate         () override;
	virtual bool IsOkayToFinishActivating() const override;

	virtual void OnFinishActivating () override;
	virtual void OnDeactivate       () override;
	virtual void OnEscPressed       () override;
	virtual void OnPausePressed     () override;
	virtual void OnNavEvent         (ENavigationKey Key) override; // todo: just for debugging
	virtual void OnPawnNavEvent     (EDefconPawnNavigationEvent Event, bool Active) override;
	virtual void OnPawnWeaponEvent  (EDefconPawnWeaponEvent Event, bool Active) override;

	virtual void OnToggleDebugStats        () override;
	virtual void OnToggleShowBoundingBoxes () override;
	virtual void OnToggleShowOrigin        () override;


	void InitMapperAndTerrain     ();
	void UpdateGameObjects        (float DeltaTime);
	void ConcludeMission          ();

	void InitPlayerShip           ();
	void KeepPlayerInView             (float DeltaTime);
	bool IsPlayerShipThrustActive () const;
	void CheckPlayerCollided      ();
	void CheckIfPlayerHit         (Defcon::CGameObjectCollection& Objects);
	void DoThrustSound            (float DeltaTime);

	void Hyperspace               ();
	void DetonateSmartbomb        ();
	void DeleteAllObjects         ();
	void OnPlayerShipDestroyed    ();
	void DestroyPlayerShip        ();

	void SpecializeMaterialization(Defcon::FMaterializationParams& Params, Defcon::EObjType ObjectType);


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayMainWidgetBase* PlayAreaMain;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayStatsWidgetBase* PlayAreaStats;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayRadarWidgetBase* PlayAreaRadar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UWidget* Fader;



	Defcon::CTerrain*              Terrain;
	Defcon::CGameObjectCollection  Enemies;
	Defcon::CGameObjectCollection  Objects;  // stuff like beacons, stars, text, etc.
	Defcon::CGameObjectCollection  Debris;
	Defcon::CGameObjectCollection  Blasts;
	Defcon::CGameObjectCollection  Powerups;

	Defcon::CScheduledTaskList     ScheduledTasks;

	Defcon::CArenaCoordMapper      MainAreaMapper;
	Defcon::CArenaCoordMapper      MainAreaStarsMapper;

	Daylon::FLoopedSound           ShipThrustSoundLoop;
	bool                           WasShipUnderThrust = false;

	float      ArenaWidth = 0.0f; // todo: this is just ArenaSize.X
	FVector2D  ArenaSize;
	FVector2D  MainAreaSize;

	float      m_fRadarFritzed      = 0.0f;
	float      FadeAge              = 0.0f;
	int32      NumPlayerPassengers  = 0;
	int32      m_nFlashScreen       = 0;//todo: is this needed?
	bool 	   bFinishActivating    = false;
	bool       bMissionDoneMsgShown = false;
	bool       AreHumansInMission   = false;
	bool       bArenaClosing        = false;
	bool       m_bRunSlow           = false;
	bool       bIsPaused            = false;


	public:

	bool                                  IsPaused           () const { return bIsPaused; }
	Defcon::I2DCoordMapper&               GetMainAreaMapper  () { return MainAreaMapper; }
	const Defcon::I2DCoordMapper&         GetMainAreaMapper  () const { return MainAreaMapper; }
	
	Defcon::CPlayerShip&                  GetPlayerShip      ();
	const Defcon::CPlayerShip&            GetPlayerShip      () const;
	Defcon::CGameObjectCollection&        GetHumans          ();
	const Defcon::CGameObjectCollection&  GetHumans          () const;
	Defcon::CGameObjectCollection&        GetObjects         () { return Objects; }
	Defcon::CGameObjectCollection&        GetEnemies         () { return Enemies; }

	float                ShortestDirection    (const CFPoint& WorldPosA, const CFPoint& WorldPosB, CFPoint& Result) const;
	void                 Lerp                 (const CFPoint& WorldPosA, const CFPoint& WorldPosB, CFPoint& Result, float T) const;
	float                Xdistance            (float WorldX1, float WorldX2) const;
	float                HorzDistance         (const CFPoint& WorldPosA, const CFPoint& WorldPosB) const { return Xdistance(WorldPosA.x, WorldPosB.x); }
	float                WrapX                (float WorldX) const;
	bool                 IsPointVisible       (const CFPoint& WorldPos) const;

	float                GetWidth             () const { return ArenaWidth; }
	float                GetHeight            () const { return ArenaSize.Y; }
	float                GetDisplayWidth      () const { return MainAreaSize.X; }
	float                GetTerrainElev       (float X) const;
	bool                 HasTerrain           () const { return true; } // todo: support no-terrain missions
	void                 CreateTerrain        ();
	void                 AddDebris            (Defcon::IGameObject* Obj);
	void                 LayMine              (Defcon::IGameObject& Obj, const CFPoint& From, int32, int32);
	Defcon::IBullet*     FireBullet           (Defcon::IGameObject&, const CFPoint& From, int32 SoundID, int32);
	bool                 IsEnding             () const { return bArenaClosing; }
	void                 DestroyObject        (Defcon::IGameObject* Obj, bool bExplode = true);
	void                 IncreaseScore        (int32 Points, bool bVis, const CFPoint* P);
	void                 CreateEnemy          (Defcon::EObjType Kind, Defcon::EObjType CreatorType, const CFPoint& Where, float Countdown, Defcon::EObjectCreationFlags Flags);
	Defcon::CEnemy*      CreateEnemyNow       (Defcon::EObjType Kind, Defcon::EObjType CreatorType, const CFPoint& Where, Defcon::EObjectCreationFlags Flags);
	Defcon::CHuman*      FindNearestHuman     (float X) const;
	Defcon::IGameObject* FindEnemy            (Defcon::EObjType Kind, Defcon::IGameObject* Obj = nullptr) const { return Enemies.Find(Kind, Obj); }
	void                 CheckIfObjectsGotHit (Defcon::CGameObjectCollection& Objects);
	void                 ShieldBonk           (Defcon::IGameObject* Obj, float Force);
	void                 ProcessWeaponsHits   ();
	void                 AddMessage           (const FString& Str, float Duration = 0.0f);
	void                 TransportPlayerShip  ();
	void                 AllStopPlayerShip    ();

	
	// Debugging support -------------------------------------------------------------------------------------

	void                 OnSpawnEnemy         ();
	void                 OnSelectEnemyToSpawn ();

	int32                SpawnedEnemyIndex = 0;
	
	Defcon::EObjType SpawnedEnemyTypes[18] =
	{
		Defcon::EObjType::LANDER,
		Defcon::EObjType::HUNTER,
		Defcon::EObjType::GUPPY,
		Defcon::EObjType::BOMBER,
		Defcon::EObjType::POD,
		Defcon::EObjType::SWARMER,
		Defcon::EObjType::BAITER,
		Defcon::EObjType::PHRED,
		Defcon::EObjType::BIGRED,
		Defcon::EObjType::MUNCHIE,
		Defcon::EObjType::FIREBOMBER_TRUE,
		Defcon::EObjType::FIREBOMBER_WEAK,
		Defcon::EObjType::DYNAMO,
		Defcon::EObjType::SPACEHUM,
		Defcon::EObjType::REFORMER,
		Defcon::EObjType::GHOST,
		Defcon::EObjType::BOUNCER_TRUE,
		Defcon::EObjType::BOUNCER_WEAK
	};
};

extern UDefconPlayViewBase* GArena; // todo: globals are bad
