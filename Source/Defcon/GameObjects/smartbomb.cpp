// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// smartbomb.cpp


#include "smartbomb.h"

#include "Common/util_core.h"
#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Globals/GameObjectResources.h"
#include "Arenas/DefconPlayViewBase.h"

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
	Type = EObjType::SMARTBOMB;

	NameColor = TEXT("Color");
	NameOs    = TEXT("Os");

	MID = Cast<UMaterialInstanceDynamic>(GameObjectResources.SmartbombBrushPtr->GetResourceObject());

	if(MID == nullptr)
	{
		auto ParentMaterial = Cast<UMaterialInterface>(GameObjectResources.SmartbombBrushPtr->GetResourceObject());
		if (ParentMaterial)
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


void Defcon::CSmartbomb::Move(float fTimeElapsed)
{
	// The smartbomb produces a shockwave that 
	// moves quickly and destroys anything on 
	// the screen.

	if(bMortal)
		return;

	Age += fTimeElapsed;

	CFPoint radius = m_range;
	float t = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	radius *= t;
	float shockwave = radius.x;

	CFPoint pt, bombpt;
	MapperPtr->To(Position, bombpt);

	IGameObject* pObj = m_pTargets->GetFirst();
	IGameObject* pObj2;

	while(pObj != nullptr)
	{
		if(pObj->CanBeInjured() 
			&& pObj->GetType() != EObjType::PLAYER
			&& pObj->GetType() != EObjType::HUMAN)
		{
			MapperPtr->To(pObj->Position, pt);
			if(pt.Distance(bombpt) < shockwave)
			{
				pObj2 = pObj->GetNext();
				gpArena->ExplodeObject(pObj);
				pObj->MarkAsDead();
				pObj = pObj2;
				continue;
			}
		}
		pObj = pObj->GetNext();
	}


	// Push debris with shockwave.

	m_pDebris->ForEach([&](IGameObject* pObj)
	{
		if(pObj->GetType() != EObjType::TEXT)
		{
			MapperPtr->To(pObj->Position, pt);
			if(pt.Distance(bombpt) < shockwave)
			{
				// Make debris pushed by shockwave.
				pt.Sub(bombpt);
				pt.Normalize();
				pt.y *= -1;
				pt.Mul(SMARTBOMB_WAVEPUSH * (1.0f - t) + SMARTBOMB_WAVEPUSHMIN);
				pObj->Orientation.Fwd += pt;
			}
		}
	});

	if(Age > SMARTBOMB_LIFESPAN)
	{
		bMortal = true;
		Lifespan = 0.0f;
	}
}


void Defcon::CSmartbomb::Draw(FPaintArguments& framebuf, const I2DCoordMapper& map)
{
	check(GameObjectResources.SmartbombBrushPtr != nullptr);

	if(MID == nullptr)
	{
		return;
	}

	CFPoint radius = m_range;
	float t = FMath::Min(1.0f, Age / SMARTBOMB_LIFESPAN);
	radius *= t;
	float shockwave = radius.x;

	CFRect r;
	map.To(Position, r.LL);
	const FVector2D pt(r.LL.x, r.LL.y);
	r.UR = r.LL;
	r.Inflate(CFPoint(shockwave, shockwave * .5f));
	int x1, y1, x2, y2;
	r.Classicize(x1, y1, x2, y2);

	// To draw in UE, we need to access the material used by the smartbomb brush.

	MID->SetScalarParameterValue(NameOs, 1.0f - t);

	const auto S = FVector2D(r.GetWidth(), r.GetHeight());
	const FSlateLayoutTransform Translation(pt - S / 2);
	const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

	FSlateDrawElement::MakeBox(
		*framebuf.OutDrawElements,
		framebuf.LayerId,
		Geometry.ToPaintGeometry(),
		GameObjectResources.SmartbombBrushPtr,
		ESlateDrawEffect::None,
		C_WHITE /* * framebuf.RenderOpacity * framebuf.InWidgetStyle->GetColorAndOpacityTint().A */);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
