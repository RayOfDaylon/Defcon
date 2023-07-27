// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconPlayRadarWidgetBase.h"
#include "Main/mapper.h"
#include "DefconUtils.h"
#include "DaylonUtils.h"


#pragma optimize("", off)

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
	Defcon::CPlayer*               Ptr, 
	const FVector2D&               MainSize, 
	int32                          ArenaWidth, 
	Defcon::CArenaCoordMapper*     InArenaCoordMapperPtr, 
	Defcon::CGameObjectCollection* ObjectsPtr, 
	Defcon::CGameObjectCollection* EnemiesPtr
)
{
	ArenaCoordMapperPtr = InArenaCoordMapperPtr;

	PlayerShipPtr = Ptr;
	PlayerShipPtr->RadarBrush = PlayerShipRadarImage;

	const auto S = Daylon::GetWidgetSize(this);
	CoordMapper.m_pPlayer = PlayerShipPtr;
	CoordMapper.m_radarSize.set((float)S.X, (float)S.Y);
	CoordMapper.Init((int32)MainSize.X, (int32)MainSize.Y, ArenaWidth);

	Objects = ObjectsPtr;
	Enemies = EnemiesPtr;
}


void UDefconPlayRadarWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
}


void UDefconPlayRadarWidgetBase::DrawObjects(const Defcon::CGameObjectCollection* Collection, const FPaintArguments& PaintArguments) const
{
	if(Collection == nullptr)
	{
		return;
	}

	Collection->ForEach([&](Defcon::IGameObject* Object)
	{
		Object->DrawSmall(const_cast<FPaintArguments&>(PaintArguments), CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));
#if 0
		const auto& ObjColor = Object->GetRadarColor();

		if(ObjColor.A == 0.0f) // todo: use a switch(Object->RadarDrawStyle) { drawDefault, drawNothing, drawCustom } and move draw code below into default DrawSmall method.
		{
			return; // todo: should call Object->DrawSmall; if it doesn't draw anything it can just override with an empty implementation.
		}

		CFPoint pt;
		CoordMapper.To(Object->m_pos, pt);

		const auto S = FVector2D(9, 9);

#if 0
		// Quantize location. -- no, makes screen jitter too much unless we quantize coord mapper
		pt.x = ROUND(pt.x / 9) * 9;
		pt.y = ROUND(pt.y / 9) * 9;
#endif

		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);

		const auto Geometry = FrameBuffer.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*FrameBuffer.OutDrawElements,
			FrameBuffer.LayerId,
			Geometry.ToPaintGeometry(),
			&RadarBrush,
			ESlateDrawEffect::None,
			Object->GetRadarColor() * FrameBuffer.RenderOpacity * FrameBuffer.InWidgetStyle->GetColorAndOpacityTint().A);

#endif
	}
	);
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

	if(Objects == nullptr)
	{
		// The game instance hasn't started yet, so don't do anything further.
		return LayerId;
	}

	auto PaintGeometry = AllottedGeometry.ToPaintGeometry();

	if(PlayerShipPtr != nullptr)
	{
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
	}

	FPaintArguments PaintArguments;

	PaintArguments.PaintGeometry    = &PaintGeometry;
	PaintArguments.AllottedGeometry = &AllottedGeometry;
	PaintArguments.Args             = &Args;
	PaintArguments.LayerId          = LayerId;
	PaintArguments.MyCullingRect    = &MyCullingRect;
	PaintArguments.OutDrawElements  = &OutDrawElements;
	PaintArguments.RenderOpacity    = 1.0f;
	PaintArguments.InWidgetStyle    = &InWidgetStyle;

	// Draw two lines showing the main arena left/right edges.
	// These should always be equidistant from the player.

	// Draw terrain first, everything else can overlay it.

	if(TerrainPtr != nullptr)
	{
		TerrainPtr->DrawSmall(PaintArguments, CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));
	}

	// Draw the arena objects. 

	DrawObjects(Objects, PaintArguments);
	DrawObjects(Enemies, PaintArguments);


	// If we have humans, draw them too.

	auto GameInstance = gDefconGameInstance;
	const auto Mission = GameInstance->GetMission();

	if(Mission != nullptr && Mission->HumansInvolved())
	{
		DrawObjects(&GameInstance->GetHumans(), PaintArguments);
	}

	
	// Draw player ship last so nothing overlays it.

	if(PlayerShipPtr != nullptr)
	{
		PlayerShipPtr->DrawSmall(PaintArguments, CoordMapper, const_cast<FSlateColorBrush&>(RadarBrush));
	}


	return LayerId;
}

#pragma optimize("", on)
