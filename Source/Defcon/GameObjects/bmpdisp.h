// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once



#include "Common/PaintArguments.h"
#include "GameObjects/gameobjlive.h"


namespace Defcon
{
	// This class is used just as an IGameObject wrapper around a manually defined UE sprite.
	// e.g. the explosion fireball that precedes the debris pushing outward.

	class CBitmapDisplayer : public IGameObject
	{
		public:
			CBitmapDisplayer();
			virtual ~CBitmapDisplayer();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			virtual void Move(float DeltaTime);

	}; // CBitmapDisplayer
}

