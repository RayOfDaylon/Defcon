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




#ifdef _DEBUG
const char* Defcon::CSmartbomb::GetClassname() const
{
	static char* psz = "Smartbomb";
	return psz;
}
#endif


Defcon::CSmartbomb::CSmartbomb()
{
	ParentType = Type;
	Type       = EObjType::SMARTBOMB;

	NameColor = TEXT("Color");
	NameOs    = TEXT("Os");

	MID = Cast<UMaterialInstanceDynamic>(GameObjectResources.SmartbombBrushPtr->GetResourceObject());

	if(MID == nullptr)
	{
		auto ParentMaterial = Cast<UMaterialInterface>(GameObjectResources.SmartbombBrushPtr->GetResourceObject());

		if(ParentMaterial != nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(ParentMaterial, nullptr);
			GameObjectResources.SmartbombBrushPtr->SetResourceObject(MID);

			const FVector4 Color(1.0f, 1.0f, 1.0f, 1.0f);
			MID->SetVectorParameterValue(NameColor, Color);
		}
		else
		{
			MID = nullptr;
		}
	}
}


void Defcon::CSmartbomb::Move(float DeltaTime)
{
	// The smartbomb produces a shockwave that 
	// moves quickly and destroys anything on the screen.

	if(bMortal)
	{
		return;
	}

	Age += DeltaTime;

	CFPoint Radius = Range;
	float T = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	Radius *= T;
	float Shockwave = Radius.x;

	CFPoint P, BombPos;
	MapperPtr->To(Position, BombPos);

	// Destroy objects within shockwave.

	Targets->ForEach([&](Defcon::IGameObject* Target)
	{
		if(Target->CanBeInjured() 
			&& Target->GetType() != EObjType::PLAYER // todo: maybe have a CanBeInjuredBySmartbomb method or generalize CanBeInjured
			&& Target->GetType() != EObjType::HUMAN)
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
		if(pObj->GetType() != EObjType::TEXT)
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


void Defcon::CSmartbomb::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	check(GameObjectResources.SmartbombBrushPtr != nullptr);

	if(MID == nullptr)
	{
		return;
	}

	const float T = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	const CFPoint Radius = Range * T;
	const float Shockwave = Radius.x;

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
		GameObjectResources.SmartbombBrushPtr,
		ESlateDrawEffect::None,
		C_WHITE /* * Painter.RenderOpacity * Painter.InWidgetStyle->GetColorAndOpacityTint().A */);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
