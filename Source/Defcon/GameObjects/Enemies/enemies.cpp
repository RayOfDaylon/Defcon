// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	enemies.cpp
	Enemy base types for Defcon game.
*/


#include "enemies.h"

#include "Common/util_color.h"
#include "Globals/_sound.h"
#include "Globals/prefs.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "GameObjects/bmpdisp.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/flak.h"

#include "Arenas/DefconPlayViewBase.h"

#include "DefconLogging.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif





Defcon::CEnemy::CEnemy()
{
	m_type = ObjType::ENEMY;
	m_fAge = 0;
	//m_bBirthsoundPlayed = false;
	m_bCanBeInjured = false;
	m_bIsCollisionInjurious = false;
	m_fBrightness = FRAND * 0.33f + 0.66f;
	//m_bMaterializes = true;
}


void Defcon::CEnemy::Init(const CFPoint& size1, const CFPoint& size2)
{
	Super::Init(size1, size2);

	// Arrange the birth debris in a circle.
	// Don't do this in ctor since we don't 
	// know where player is yet.
#if 0
	int n = array_size(m_birthDebrisLocs);

	for(int i = 0; i < n; i++)
	{
		//m_debrisPow[i] = FRAND * 2 + 0.5f;
		m_debrisPow[i] = FRAND * 8 + 0.1f;
		float u = (float)i / n;
		u *= (float)TWO_PI;

		float fRad = FRAND * ENEMY_BIRTHDEBRISDIST;
		float fw = m_arenasize.x;
				
		m_birthDebrisLocsOrg[i].Set((float)cos(u) * fRad, (float)sin(u) * fRad * .66f);
		m_birthDebrisLocsOrg[i] += m_pos;
		if(m_birthDebrisLocsOrg[i].x < 0)
			m_birthDebrisLocsOrg[i].x += fw;
		else if(m_birthDebrisLocsOrg[i].x >= fw)
			m_birthDebrisLocsOrg[i].x -= fw;
	}
#endif
}


void Defcon::CEnemy::Notify(Defcon::Message msg, void* pObj)
{
	ILiveGameObject::Notify(msg, pObj);
}


Defcon::CEnemy::~CEnemy()
{
}


void Defcon::CEnemy::OnAboutToDie() {}



void Defcon::CEnemy::Move(float f)
{
	CFPoint temp = m_inertia;
	Super::Move(f);
	m_inertia = temp;
}


void Defcon::CEnemy::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}

#if 0
void Defcon::CEnemy::DrawMaterialization(FPaintArguments& framebuf, const I2DCoordMapper& mapper) const
{
	CFPoint pt;

	const int n = array_size(m_birthDebrisLocs);

	const float t = m_materializationAge / ENEMY_BIRTHDURATION;

	const auto LayerId = framebuf.LayerId + 1;

	const auto Os = framebuf.RenderOpacity * framebuf.InWidgetStyle->GetColorAndOpacityTint().A;

	for(int i = 0; i < n; i++)
	{
		mapper.To(m_birthDebrisLocs[i], pt);

		// Don't draw if particle out of frame.
		if(pt.x < 0 || pt.x >= framebuf.GetWidth())
		{
			//UE_LOG(LogGame, Warning, TEXT("Materialization particle out of frame at %d"), (int32)pt.x);
			continue;
		}

		//UE_LOG(LogGame, Warning, TEXT("Materialization particle visible at %d"), (int32)pt.x);

		FLinearColor Color;
		LerpColor(C_BLACK, C_WHITE, t, Color);

		const auto S = FVector2D(LERP(3.0f, 9.0f, t)); // todo: could halve it here
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = framebuf.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*framebuf.OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(),
			GameObjectResources.DebrisBrushRoundPtr,
			ESlateDrawEffect::None,
			Color * Os);
	}
}
#endif


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
