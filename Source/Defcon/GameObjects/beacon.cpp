// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "beacon.h"

#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"
#include "Main/mapper.h"
#include "DefconLogging.h"


Defcon::CBeacon::CBeacon()
{
	m_parentType = m_type;
	m_type       = ObjType::BEACON;
	m_bMortal    = false;

	CreateSprite(ObjType::BEACON);
}


Defcon::CBeacon::~CBeacon()
{
}


void Defcon::CBeacon::Move(float DeltaTime)
{
	// Beacons don't move, but they do flash, and we want to show that in the radar color.

	// Oscillate between yellow and brown.

	m_fAge += DeltaTime;

	float T = (sin(m_fAge * PI * 2) + 1) * 0.5f;

	LerpColor(C_YELLOW, C_BROWN, T, m_smallColor);
}


void Defcon::CBeacon::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
}
