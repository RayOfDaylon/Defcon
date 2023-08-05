// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "gameobjlive.h"


namespace Defcon
{
	class CBeacon : public ILiveGameObject
	{
		typedef ILiveGameObject Super;

		public:
			CBeacon();
			~CBeacon();

			virtual void    Move   (float DeltaTime) override;
	};
}
