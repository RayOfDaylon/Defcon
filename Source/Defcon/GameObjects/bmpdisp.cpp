// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

// bmpdisp.cpp
// Implements CBitmapDisplayer game object


#include "bmpdisp.h"
#include "Common/util_core.h"
#include "Common/gamma.h"
#include "Main/mapper.h"
#include "DefconLogging.h"


Defcon::CBitmapDisplayer::CBitmapDisplayer()
{
	ParentType = Type;
	Type       = EObjType::TEXTURE;

	Orientation.Fwd.Set(0.0f, 0.0f);
	bMortal       = true;
	bCanBeInjured = false;
	Lifespan      = 100.0f;
	Age           = 0.0f;
}


Defcon::CBitmapDisplayer::~CBitmapDisplayer()
{
}

#ifdef _DEBUG
const char* Defcon::CBitmapDisplayer::GetClassname() const
{
	static char* psz = "Texture";
	return psz;
}
#endif


void Defcon::CBitmapDisplayer::Move(float DeltaTime)
{
	Age += DeltaTime;
	Position.MulAdd(Orientation.Fwd, DeltaTime);
}
