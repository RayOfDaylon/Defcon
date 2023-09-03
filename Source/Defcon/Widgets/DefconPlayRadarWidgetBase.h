// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconPlayWidgetBase.h"
#include "GameObjects/Auxiliary/stargate.h"
#include "GameObjects/terrain.h"
#include "GameObjects/playership.h"
#include "Main/mapper.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "DefconPlayRadarWidgetBase.generated.h"


/*
	This widget displays the radar screen, normally above the main gameplay viewport.
*/
UCLASS()
class DEFCON_API UDefconPlayRadarWidgetBase : public UDefconPlayWidgetBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

	virtual int32 NativePaint(
		const FPaintArgs&          Args,
		const FGeometry&           AllottedGeometry,
		const FSlateRect&          MyCullingRect,
		FSlateWindowElementList&   OutDrawElements,
		int32                      LayerId,
		const FWidgetStyle&        InWidgetStyle,
		bool                       bParentEnabled) const override;

	void DrawObjects(const Defcon::CGameObjectCollection* Collection, const FPainter&) const;


	FSlateColorBrush RadarBrush = FSlateColorBrush(C_WHITE);

	Defcon::CRadarCoordMapper        CoordMapper;

	Defcon::CArenaCoordMapper*       ArenaCoordMapperPtr = nullptr;
	Defcon::CTerrain*                TerrainPtr          = nullptr;
	Defcon::CPlayerShip*             PlayerShipPtr       = nullptr;
	Defcon::CGameObjectCollection*   Objects             = nullptr;
	Defcon::CGameObjectCollection*   Enemies             = nullptr;


	public:

	void OnFinishActivating ();
	void OnDeactivate       ();

	void Init           (Defcon::CPlayerShip* PlayerShip, const FVector2D& MainSize, int32 ArenaWidth, Defcon::CArenaCoordMapper* ArenaCoordMapper, 
							Defcon::CGameObjectCollection* Objects, Defcon::CGameObjectCollection* Enemies);

	void SetTerrain     (Defcon::CTerrain* Ptr);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Textures)
	FSlateBrush PlayerShipRadarImage;
};


