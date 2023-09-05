// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconPlayWidgetBase.h"
#include "GameObjects/terrain.h"
#include "GameObjects/playership.h"
#include "Widgets/SDefconTerrain.h"
#include "Main/mapper.h"
#include "Common/util_core.h"
#include "DaylonUtils.h"
#include "DefconPlayMessagesWidgetBase.h"
#include "Runtime/UMG/Public/Components/CheckBox.h"
#include "DefconPlayMainWidgetBase.generated.h"

/*
	This widget displays various runtime stats useful for debugging.
	It normally appears in the upper left corner of the main game viewport.
*/
UCLASS()
class DEFCON_API UDefconPlayerShipDebugWidgetBase : public UDefconPlayWidgetBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Position;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* OrientFwd;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Speed;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Thrust;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ThrustLeftKey;
	   
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ThrustRightKey;
   
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ThrustUpKey;
   
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ThrustDownKey;
 
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* Duration;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ShieldLevel;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* EnemyCounts;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* HumansLeft;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DebrisCount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UCheckBox* TraceHiddenEnemies;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* HiddenEnemyCount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UCheckBox* TraceOutOfBoundEnemies;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* OutOfBoundEnemyCount;

	public:

	Defcon::CPlayerShip*             PlayerShipPtr = nullptr;
	Defcon::CGameObjectCollection*   Enemies       = nullptr;
	Defcon::CGameObjectCollection*   Debris        = nullptr;
	float                            ArenaHeight   = 0.0f;
};


struct FStar
{
	FLinearColor   Color;
	CFPoint        P;
	float          BlinkSpeed;
	bool           IsBig;
	float          Age = 0.0f;

	FStar()
	{
		Age        = Daylon::FRandRange(0.0f, 2.0f);
		BlinkSpeed = Daylon::FRandRange(0.5f, 2.0f);
		IsBig      = BRAND;

		const FLinearColor Colors[] = 
		{
			C_DARK, 
			C_RED,
			C_YELLOW,
			C_BLUE,
			C_LIGHTYELLOW,
			C_LIGHTBLUE
		};

		Color = Colors[IRAND(array_size(Colors))];
	}


	void Update(float DeltaTime)
	{
		Age += DeltaTime;
	}
};


/*
	This widget displays the main gameplay viewport for the play arena.
	It's where "all the action takes place" when playing a mission.
*/
UCLASS()
class DEFCON_API UDefconPlayMainWidgetBase : public UDefconPlayWidgetBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	virtual int32 NativePaint(
		const FPaintArgs&          Args,
		const FGeometry&           AllottedGeometry,
		const FSlateRect&          MyCullingRect,
		FSlateWindowElementList&   OutDrawElements,
		int32                      LayerId,
		const FWidgetStyle&        InWidgetStyle,
		bool                       bParentEnabled) const override;


	// Texture atlas for player ship when facing left
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> StargateAtlas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Textures)
	FSlateBrush StarBrush;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> DestroyedPlayerAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BeaconAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> HeartAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> ExplosionAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> Explosion2Atlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> LanderAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BomberAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BomberLeftAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> FirebomberAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> WeakFirebomberAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> PodAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BaiterAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> MineAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> SwarmerAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> DynamoAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> SpacehumAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> PhredAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BigRedAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> MunchieAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> GuppyAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> HunterAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> GhostDemoAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> GhostPartAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> ReformerDemoAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> ReformerPartAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> BulletAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> ThinBulletAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> FireballAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> LaserbeamAtlas;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> PlayerShipExhaustAtlas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	TObjectPtr<UDaylonSpriteWidgetAtlas> PowerupInvincibilityAtlas;
	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	FSlateBrush SmartbombBrush;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	FSlateBrush DebrisBrushRound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Textures)
	FSlateBrush DebrisBrushSquare;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayerShipDebugWidgetBase* PlayerShipDebug;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UDefconPlayMessagesWidgetBase* Messages;


	TWeakPtr<Daylon::SpritePlayObject2D> PlayerShipExhaust;

	
	bool bShowBoundingBoxes = false;
	bool bShowOrigin        = false;

	void UpdatePlayerShip(float DeltaTime);

	void DrawObjects    (const Defcon::CGameObjectCollection* Collection, FPainter& PaintArgs) const;
	void DrawObjectBbox (Defcon::IGameObject* Object, FPainter& PaintArgs) const;


	bool bDoneActivating     = false;
	bool bSafeToStart        = false;
	bool AreHumansInMission  = false;

	TSharedPtr<SDefconTerrain> TerrainWidget;
	Defcon::CTerrain*          Terrain = nullptr;


	public:

	void OnFinishActivating  ();
	void OnMissionStarting   ();

	void Init(Defcon::CGameObjectCollection* Humans, 
			  Defcon::CGameObjectCollection* Objects, 
			  Defcon::CGameObjectCollection* Enemies, 
			  Defcon::CGameObjectCollection* Debris,
			  Defcon::CGameObjectCollection* Blasts,
			  const FVector2D& ArenaSize);

	void SetTerrain(Defcon::CTerrain* Terrain);

	void SetSafeToStart(bool b = true) { bSafeToStart = b; }

	void OnDeactivate();

	void OnToggleDebugStats        ();
	void OnToggleShowBoundingBoxes ();
	void OnToggleShowOrigin        ();


	void AddMessage    (const FString& Str, float Duration = 0.0f);
	void ClearMessages ();

	Defcon::I2DCoordMapper*          CoordMapperPtr      = nullptr;
	Defcon::I2DCoordMapper*          CoordMapperStarsPtr = nullptr;
	Defcon::I2DCoordMapper*          CoordMapperRadarPtr = nullptr;
	Defcon::CPlayerShip*             PlayerShipPtr       = nullptr;
	Defcon::CGameObjectCollection*   Objects             = nullptr;
	Defcon::CGameObjectCollection*   Enemies             = nullptr;
	Defcon::CGameObjectCollection*   Debris              = nullptr;
	Defcon::CGameObjectCollection*   Blasts              = nullptr;
	Defcon::CGameObjectCollection*   Humans              = nullptr;

	FVector2D                        ArenaSize           = FVector2D(0.0f, 0.0f);

	TArray<FStar>                    Stars;
};


