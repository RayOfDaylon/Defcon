// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayRadarWidgetBase.h"
//#include "DefconGameInstance.h"
#include "Main/mapper.h"
#include "DefconUtils.h"
#include "DaylonUtils.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




void UDefconPlayRadarWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//RadarBrush = FSlateColorBrush(C_WHITE);
}


void UDefconPlayRadarWidgetBase::SetTerrain(Defcon::CTerrain* Ptr)
{
	TerrainPtr = Ptr;
}


void UDefconPlayRadarWidgetBase::OnFinishActivating()
{
}


void UDefconPlayRadarWidgetBase::OnDeactivate()
{
}


void UDefconPlayRadarWidgetBase::Init
(
	const FVector2D&               MainSize, 
	int32                          ArenaWidth, 
	Defcon::CArenaCoordMapper*     InArenaCoordMapperPtr, 
	Defcon::CGameObjectCollection* ObjectsPtr, 
	Defcon::CGameObjectCollection* EnemiesPtr
)
{
	ArenaCoordMapperPtr = InArenaCoordMapperPtr;

	auto PlayerShipPtr = &Defcon::GGameMatch->GetPlayerShip();
	
	PlayerShipPtr->RadarBrush = PlayerShipRadarImage;

	const auto S = Daylon::GetWidgetSize(this);
	CoordMapper.Player = PlayerShipPtr;
	CoordMapper.RadarSize.Set((float)S.X, (float)S.Y);
	CoordMapper.Init((int32)MainSize.X, (int32)MainSize.Y, ArenaWidth);

	Objects = ObjectsPtr;
	Enemies = EnemiesPtr;
}


void UDefconPlayRadarWidgetBase::DrawObjects(const Defcon::CGameObjectCollection* Collection, const FPainter& Painter) const
{
	if(Collection == nullptr)
	{
		return;
	}

	Collection->ForEach([&](Defcon::IGameObject* Object)
	{
		Object->DrawSmall(const_cast<FPainter&>(Painter), CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));
#if 0
		// Quantize location. -- no, makes screen jitter too much unless we quantize coord mapper
		pt.x = ROUND(pt.x / 9) * 9;
		pt.y = ROUND(pt.y / 9) * 9;
#endif
	});
}


int32 UDefconPlayRadarWidgetBase::NativePaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	LayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);


	if(Objects == nullptr || !GDefconGameInstance->MatchInProgress())
	{
		// The game instance hasn't started yet, so don't do anything further.
		return LayerId;
	}

	auto PaintGeometry = AllottedGeometry.ToPaintGeometry();



	// Draw two lines showing the main arena left/right edges.
	// These should always be equidistant from the player (wait, what?)

	const float ArenaHalfWidth = CoordMapper.GetScreenSize().x / 2;

	CFPoint pt;
	CFPoint ps(ArenaCoordMapperPtr->GetOffset());
	CoordMapper.To(ps, pt);
		
	TArray<FVector2f> LinePts;
	LinePts.Add(FVector2f(pt.x, 0.0f));
	LinePts.Add(FVector2f(pt.x, 200.0f));

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, PaintGeometry, LinePts, ESlateDrawEffect::None, C_DARKER, true, 2.0f);

	ps.x += CoordMapper.GetScreenSize().x;
	CoordMapper.To(ps, pt);
	LinePts[0].X = 
	LinePts[1].X = pt.x;

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, PaintGeometry, LinePts, ESlateDrawEffect::None, C_DARKER, true, 2.0f);


	// Draw other stuff

	FPainter Painter;

	Painter.PaintGeometry    = &PaintGeometry;
	Painter.AllottedGeometry = &AllottedGeometry;
	Painter.Args             = &Args;
	Painter.LayerId          = LayerId;
	Painter.MyCullingRect    = &MyCullingRect;
	Painter.OutDrawElements  = &OutDrawElements;
	Painter.RenderOpacity    = InWidgetStyle.GetColorAndOpacityTint().A;
	//Painter.InWidgetStyle    = &InWidgetStyle;


	// Draw terrain first, everything else can overlay it.

	if(TerrainPtr != nullptr)
	{
		TerrainPtr->DrawSmall(Painter, CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));
	}

	// Draw the arena objects. 

	DrawObjects(Objects, Painter);
	DrawObjects(Enemies, Painter);


	// If we have humans, draw them too.

	const auto Mission = Defcon::GGameMatch->GetMission();

	if(Mission != nullptr && Mission->HumansInvolved())
	{
		DrawObjects(&Defcon::GGameMatch->GetHumans(), Painter);
	}

	
	// Draw player ship last so nothing overlays it.

	Defcon::GGameMatch->GetPlayerShip().DrawSmall(Painter, CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));

	return LayerId;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
