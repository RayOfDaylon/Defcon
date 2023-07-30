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
#include "Main/event.h"
#include "DefconPlayViewBase.generated.h"



namespace Defcon
{
	class IBullet;
}

struct FDefconInputState
{
	bool    bActive   = false;
	double  fTimeDown = 0.0;
};

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

	friend class Defcon::CEndMissionEvent;

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
	virtual void OnNavEvent         (ENavigationKey Key) override; // todo: just for debugging
	virtual void OnPawnNavEvent     (EDefconPawnNavigationEvent Event, bool Active) override;
	virtual void OnPawnWeaponEvent  (EDefconPawnWeaponEvent Event, bool Active) override;

	virtual void OnToggleDebugStats        () override;
	virtual void OnToggleShowBoundingBoxes () override;
	virtual void OnToggleShowOrigin        () override;


	void InitMapperAndTerrain   ();
	void InitPlayerShip         ();
	void SettlePlayer           (float DeltaTime);
	void DoThrustSound          (float DeltaTime);
	bool IsPlayerShipThrustActive() const;
	void UpdatePlayerShipInputs ();
	void UpdateGameObjects      (float DeltaTime);
	void ConcludeMission        ();

	void CheckPlayerCollided    ();
	void CheckIfPlayerHit       (Defcon::CGameObjectCollection& Objects);

	void Hyperspace             ();
	void FireSmartbomb          ();
	void DeleteAllObjects       ();
	void OnPlayerShipDestroyed  ();
	void DestroyPlayerShip      ();

	void SpecializeMaterialization(Defcon::FMaterializationParams& Params, Defcon::ObjType ObjectType);


	// todo: do we need these?
	Defcon::CGameObjectCollection& GetDebris  () { return m_debris; }



	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayMainWidgetBase* PlayAreaMain;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayStatsWidgetBase* PlayAreaStats;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayRadarWidgetBase* PlayAreaRadar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UWidget* Fader;





	Defcon::CTerrain*              Terrain;
	Defcon::CGameObjectCollection  m_enemies;
	Defcon::CGameObjectCollection  m_objects;  // stuff like beacons, stars, text, etc.
	Defcon::CGameObjectCollection  m_debris;
	Defcon::CGameObjectCollection  m_blasts;

	Defcon::CEventQueue            Events;


	Defcon::CArenaCoordMapper      MainAreaMapper;

	FDefconInputState     MoveShipUpState;
	FDefconInputState     MoveShipDownState;
	FDefconInputState     MoveShipLeftState;
	FDefconInputState     MoveShipRightState;

	Daylon::FLoopedSound  ShipThrustSoundLoop;
	bool                  WasShipUnderThrust = false;

	float     ArenaWidth = 0.0f; // todo: this is just ArenaSize.X
	FVector2D ArenaSize;
	FVector2D MainAreaSize;

	bool 	bFinishActivating = false;

	bool    m_bHumansInMission = false;
	bool    m_bArenaDying    = false;
	bool    m_bRunSlow       = false;
	float   m_fRadarFritzed  = 0.0f;
	float   m_fFadeAge       = 0.0f;
	int32   m_nFlashScreen   = 0;//todo: is this needed?


	public:

	Defcon::I2DCoordMapper&               GetMainAreaMapper       () { return MainAreaMapper; }
	const Defcon::I2DCoordMapper&         GetConstMainAreaMapper  () const { return MainAreaMapper; }
	
	Defcon::CPlayer&                      GetPlayerShip           ();
	const Defcon::CGameObjectCollection&  GetConstHumans          () const;
	Defcon::CGameObjectCollection&        GetHumans               ();
	Defcon::CGameObjectCollection&        GetObjects              () { return m_objects; }
	Defcon::CGameObjectCollection&        GetEnemies              () { return m_enemies; }

	float                Direction            (const CFPoint& posA, const CFPoint& posB,CFPoint& result) const;
	void                 Lerp                 (const CFPoint& pt1, const CFPoint& pt2, CFPoint& result, float t) const;
	float                Xdistance            (float x1, float x2) const;
	float                HorzDistance         (const CFPoint& ptFrom, const CFPoint& ptTo) const { return Xdistance(ptFrom.x, ptTo.x); }
	float                WrapX                (float x) const;
	bool                 IsPointVisible       (const CFPoint& pt) const;

	float                GetWidth             () const { return ArenaWidth; }
	float                GetHeight            () const { return ArenaSize.Y; }
	float                GetDisplayWidth      () const { return MainAreaSize.X; }
	float                GetTerrainElev       (float X) const;
	bool                 HasTerrain           () const { return true; } // todo: support no-terrain missions
	void                 CreateTerrain        ();
	void                 AddDebris            (Defcon::IGameObject* p);
	void                 LayMine              (Defcon::IGameObject& obj, const CFPoint& from, int, int);
	Defcon::IBullet*     FireBullet           (Defcon::IGameObject&, const CFPoint& From, int SoundID, int);
	bool                 IsEnding             () const { return m_bArenaDying; }
	void                 ExplodeObject        (Defcon::IGameObject* pObj);
	void                 IncreaseScore        (int32 Points, bool bVis, const CFPoint* pPos);
	void                 CreateEnemy          (Defcon::ObjType kind, const CFPoint& where, float When, bool bMaterializes, bool bTarget);
	Defcon::CEnemy*      CreateEnemyNow       (Defcon::ObjType kind, const CFPoint& where, bool bMaterializes, bool bTarget);
	Defcon::IGameObject* FindHuman            (float x) const;
	Defcon::IGameObject* FindEnemy            (Defcon::ObjType t, Defcon::IGameObject* p = nullptr) const { return m_enemies.Find(t, p); }
	void                 CheckIfObjectsGotHit (Defcon::CGameObjectCollection& Objects);
	void                 ShieldBonk           (Defcon::IGameObject* pObj, float Force);
	void                 ProcessWeaponsHits   ();
	void                 AddMessage           (const FString& Str, float Duration = 0.0f);
	void                 TransportPlayerShip  ();
	void                 AllStopPlayerShip    ();
	void                 OnSpawnEnemy         (); // debug routine

};

extern UDefconPlayViewBase* gpArena; // todo: globals are bad
