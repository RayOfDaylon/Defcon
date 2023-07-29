// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Runtime/Core/Public/Logging/LogMacros.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);



#if 0
	#if WITH_EDITOR
		#define LOG_UWIDGET_FUNCTION	UE_LOG(LogGame, Log, TEXT("%S: design time: %d"), __FUNCTION__, IsDesignTime());
	#else
		#define LOG_UWIDGET_FUNCTION	UE_LOG(LogGame, Log, TEXT("%S: design time: %d"), __FUNCTION__);
	#endif
#else
	#define LOG_UWIDGET_FUNCTION
#endif

