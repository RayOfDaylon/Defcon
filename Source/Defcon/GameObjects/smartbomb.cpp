// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// smartbomb.cpp


#include "smartbomb.h"

#include "Common/util_core.h"
#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"
#include "GameObjects/Enemies/pod.h"
#include "DefconGameInstance.h"

#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


Defcon::CSmartbombShockwave::CSmartbombShockwave()
{
	ParentType = Type;
	Type       = EObjType::SMARTBOMB;
	
	static int32 SmartbombShockwaveID = 100000;

	SetID(SmartbombShockwaveID++);

	NameColor = TEXT("Color");
	NameOs    = TEXT("Os");

	MID = Cast<UMaterialInstanceDynamic>(GGameObjectResources.SmartbombShockwaveBrushPtr->GetResourceObject());

	if(MID == nullptr)
	{
		auto ParentMaterial = Cast<UMaterialInterface>(GGameObjectResources.SmartbombShockwaveBrushPtr->GetResourceObject());

		if(ParentMaterial != nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
			GGameObjectResources.SmartbombShockwaveBrushPtr->SetResourceObject(MID);

			const FVector4 Color(1.0f, 1.0f, 1.0f, 1.0f);
			MID->SetVectorParameterValue(NameColor, Color);
		}
		else
		{
			MID = nullptr;
		}
	}
}


void Defcon::CSmartbombShockwave::InitSmartbombShockwave
(
	const CFPoint&         InRange, 
	const I2DCoordMapper*  InMapper,
	CGameObjectCollection* InTargets, 
	CGameObjectCollection* InDebris
)
{
	check(InTargets != nullptr && InDebris != nullptr);

	// Install mapper explicitly because this game object class isn't instantiated normally.
	MapperPtr = const_cast<I2DCoordMapper*>(InMapper);

	Range   = InRange * SMARTBOMB_RADIUS_FACTOR;
	Targets = InTargets;
	Debris  = InDebris;

	if(!GGameMatch->GetMission()->IsMilitary())
	{
		return;
	}


	// Compute stuff for possible pod intersection.

	const auto Mission = static_cast<CMilitaryMission*>(GGameMatch->GetMission());

	const int32 NumActivePods = Targets->CountOf(EObjType::POD);

	if(NumActivePods != Mission->StartingPodCount())
	{
		return;
	}

	// Iterate the pods and see if the shockwave can reach them all.

	DestroyingPodIntersection = true;

	int32 NumPodsDestroyable = 0;
	int32 NumPodsCounted     = 0;

	CFPoint ScreenPos;
	MapperPtr->To(Position, ScreenPos);

	Targets->ForEachUntil([&ScreenPos, &NumActivePods, &NumPodsCounted, &NumPodsDestroyable, this](IGameObject* Obj)
	{
		if(Obj->GetType() != EObjType::POD)
		{
			return true;
		}

		CFPoint PodScreenPos;
		MapperPtr->To(Obj->Position, PodScreenPos);

		const auto PodInertiaX = Obj->Inertia.x;

		// Don't use the sine wave nature of the pod's inertia.
		// todo: not perfectly accurate.
		const auto PodFuturePositionX = PodScreenPos + (CFPoint(Obj->Inertia.x, 0.0f) * SMARTBOMB_LIFESPAN);

		if(ScreenPos.Distance(PodFuturePositionX) > Range.x)
		{
			DestroyingPodIntersection = false;
			return false;
		}

		NumPodsCounted++;

		return (NumPodsCounted != NumActivePods);
	});
}


void Defcon::CSmartbombShockwave::Tick(float DeltaTime)
{
	// The smartbomb produces a shockwave that 
	// moves quickly and destroys anything on the screen.

	if(bMortal)
	{
		return;
	}

	Age += DeltaTime;

	const auto T         = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	const auto Radius    = Range * T;
	const auto Shockwave = Radius.x;

	CFPoint TargetScreenPos, BombScreenPos;
	MapperPtr->To(Position, BombScreenPos);

	// Destroy objects within shockwave.

	Targets->ForEach([&](Defcon::IGameObject* Target)
	{
		if(Target->CanBeInjuredBySmartbomb(Id) && Target->CanBeInjured())
		{
			MapperPtr->To(Target->Position, TargetScreenPos);

			if(TargetScreenPos.Distance(BombScreenPos) < Shockwave)
			{
				GDefconGameInstance->GetStats().HostilesDestroyedBySmartbomb++;

				if(Target->GetType() == EObjType::POD)
				{
					if(DestroyingPodIntersection)
					{
						Target->SetDeathStyle(EDeathStyle::IncludeCrew);
					}
				}

				Target->SetKillerType(EObjType::SMARTBOMB);
				Target->SetKillerId(Id);
				GArena->DestroyObject(Target);
			}
		}
	});

	// Push debris with shockwave.

	Debris->ForEach([&](IGameObject* pObj)
	{
		//if(pObj->GetType() != EObjType::TEXT) // we have no such objects, and "TEXT" conflicts with UE's TEXT macro anyway
		{
			MapperPtr->To(pObj->Position, TargetScreenPos);

			if(TargetScreenPos.Distance(BombScreenPos) < Shockwave)
			{
				auto InertiaToImpart = TargetScreenPos - BombScreenPos;

				InertiaToImpart.NormalizeSafe();

				InertiaToImpart.y *= -1; // Flip from screen space to cartesian
				InertiaToImpart.Mul(SMARTBOMB_WAVEPUSH * (1.0f - T) + SMARTBOMB_WAVEPUSHMIN);
				pObj->Orientation.Fwd += InertiaToImpart;
			}
		}
	});

	if(Age > SMARTBOMB_LIFESPAN)
	{
		bMortal = true;
		Lifespan = 0.0f;
	}
}


void Defcon::CSmartbombShockwave::Draw(FPainter& Painter, const I2DCoordMapper& InMapper)
{
	check(GGameObjectResources.SmartbombShockwaveBrushPtr != nullptr);

	if(MID == nullptr)
	{
		return;
	}

	const auto T         = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	const auto Radius    = Range * T;
	const auto Shockwave = Radius.x;

	CFRect R;
	InMapper.To(Position, R.LL);
	const FVector2D P(R.LL.x, R.LL.y);
	R.UR = R.LL;
	R.Inflate(CFPoint(Shockwave, Shockwave * 0.5f));
	int32 x1, y1, x2, y2;
	R.Classicize(x1, y1, x2, y2);

	// To draw in UE, we need to access the material used by the smartbomb brush.

	MID->SetScalarParameterValue(NameOs, 1.0f - T);

	const auto S = FVector2D(R.GetWidth(), R.GetHeight());
	const FSlateLayoutTransform Translation(P - S / 2);
	const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*Painter.OutDrawElements,
		Painter.LayerId,
		Geometry.ToPaintGeometry(),
		GGameObjectResources.SmartbombShockwaveBrushPtr,
		ESlateDrawEffect::None,
		C_WHITE /* * Painter.RenderOpacity * Painter.InWidgetStyle->GetColorAndOpacityTint().A */);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
