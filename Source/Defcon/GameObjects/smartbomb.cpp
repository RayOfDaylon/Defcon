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


#pragma optimize("", off)



#ifdef _DEBUG
const char* Defcon::CSmartbomb::GetClassname() const
{
	static char* psz = "Smartbomb";
	return psz;
}
#endif


Defcon::CSmartbomb::CSmartbomb()
{
	m_parentType = m_type;
	m_type = ObjType::SMARTBOMB;

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

	if(m_bMortal)
		return;

	m_fAge += fTimeElapsed;

	CFPoint radius = m_range;
	float t = FMath::Min(1.0f, m_fAge / SMARTBOMB_LIFESPAN);
	radius *= t;
	float shockwave = radius.x;

	CFPoint pt, bombpt;
	m_pMapper->To(m_pos, bombpt);

	IGameObject* pObj = m_pTargets->GetFirst();
	IGameObject* pObj2;

	while(pObj != nullptr)
	{
		if(pObj->CanBeInjured() 
			&& pObj->GetType() != ObjType::PLAYER
			&& pObj->GetType() != ObjType::HUMAN)
		{
			m_pMapper->To(pObj->m_pos, pt);
			if(pt.distance(bombpt) < shockwave)
			{
				pObj2 = pObj->GetNext();
				m_pArena->ExplodeObject(pObj);
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
		if(pObj->GetType() != ObjType::TEXT)
		{
			m_pMapper->To(pObj->m_pos, pt);
			if(pt.distance(bombpt) < shockwave)
			{
				// Make debris pushed by shockwave.
				pt.sub(bombpt);
				pt.normalize();
				pt.y *= -1;
				pt.mul(SMARTBOMB_WAVEPUSH * (1.0f - t) + SMARTBOMB_WAVEPUSHMIN);
				pObj->m_orient.fwd += pt;
			}
		}
	});

	if(m_fAge > SMARTBOMB_LIFESPAN)
	{
		m_bMortal = true;
		m_fLifespan = 0.0f;
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
	float t = FMath::Min(1.0f, m_fAge / SMARTBOMB_LIFESPAN);
	radius *= t;
	float shockwave = radius.x;

	CFRect r;
	map.To(m_pos, r.LL);
	const FVector2D pt(r.LL.x, r.LL.y);
	r.UR = r.LL;
	r.inflate(CFPoint(shockwave, shockwave * .5f));
	int x1, y1, x2, y2;
	r.classicize(x1, y1, x2, y2);

	// To draw in UE, we need to access the material used by the smartbomb brush.

	MID->SetScalarParameterValue(NameOs, 1.0f - t);

	const auto S = FVector2D(r.getwidth(), r.getheight());
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


#pragma optimize("", on)
