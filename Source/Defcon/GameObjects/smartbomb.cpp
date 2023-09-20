// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// smartbomb.cpp


#include "smartbomb.h"

#include "Common/util_core.h"
#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"
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

	NameColor = TEXT("Color");
	NameOs    = TEXT("Os");

	MID = Cast<UMaterialInstanceDynamic>(GGameObjectResources.SmartbombBrushPtr->GetResourceObject());

	if(MID == nullptr)
	{
		auto ParentMaterial = Cast<UMaterialInterface>(GGameObjectResources.SmartbombBrushPtr->GetResourceObject());

		if(ParentMaterial != nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
			GGameObjectResources.SmartbombBrushPtr->SetResourceObject(MID);

			const FVector4 Color(1.0f, 1.0f, 1.0f, 1.0f);
			MID->SetVectorParameterValue(NameColor, Color);
		}
		else
		{
			MID = nullptr;
		}
	}
}


CFPoint Defcon::CSmartbombShockwave::GetBlastRadius() const
{
	// If we scale the Range (which is a diameter) by 0.5 to get the blast radius,
	// it's technically correct but feels underpowered i.e. there'll be 
	// visible enemies that don't blow up because the player ship is 
	// usually moving fast and brings new enemies into view while the 
	// shockwave is receding behind and not reaching them. So we fudge 
	// using a preference where 0.5f is technically correct but higher 
	// values can feel better.

	return Range * SMARTBOMB_RADIUS_FACTOR;
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
	const auto Radius    = GetBlastRadius() * T;
	const auto Shockwave = Radius.x;

	CFPoint P, BombPos;
	MapperPtr->To(Position, BombPos);

	// Destroy objects within shockwave.

	Targets->ForEach([&](Defcon::IGameObject* Target)
	{
		if(Target->CanBeInjuredBySmartbomb() && Target->CanBeInjured())
		{
			MapperPtr->To(Target->Position, P);

			if(P.Distance(BombPos) < Shockwave)
			{
				GDefconGameInstance->GetStats().HostilesDestroyedBySmartbomb++;

				GArena->DestroyObject(Target);
			}
		}
	});

	// Push debris with shockwave.

	Debris->ForEach([&](IGameObject* pObj)
	{
		//if(pObj->GetType() != EObjType::TEXT) // we have no such objects, and "TEXT" conflicts with UE's TEXT macro anyway
		{
			MapperPtr->To(pObj->Position, P);

			if(P.Distance(BombPos) < Shockwave)
			{
				// Make debris pushed by shockwave.
				P.Sub(BombPos);
				P.Normalize();
				P.y *= -1;
				P.Mul(SMARTBOMB_WAVEPUSH * (1.0f - T) + SMARTBOMB_WAVEPUSHMIN);
				pObj->Orientation.Fwd += P;
			}
		}
	});

	if(Age > SMARTBOMB_LIFESPAN)
	{
		bMortal = true;
		Lifespan = 0.0f;
	}
}


void Defcon::CSmartbombShockwave::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	check(GGameObjectResources.SmartbombBrushPtr != nullptr);

	if(MID == nullptr)
	{
		return;
	}

	const auto T = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	const auto Radius = GetBlastRadius() * T;
	const auto Shockwave = Radius.x;

	CFRect R;
	Mapper.To(Position, R.LL);
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
		GGameObjectResources.SmartbombBrushPtr,
		ESlateDrawEffect::None,
		C_WHITE /* * Painter.RenderOpacity * Painter.InWidgetStyle->GetColorAndOpacityTint().A */);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
