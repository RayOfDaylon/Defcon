// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconUtils.h"
#include "DefconLogging.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#if 0
UDefconGameInstance* UDefconUtils::GetGameInstance(const UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
	{
		UE_LOG(LogGame, Error, TEXT("%S: invalid WorldContextObject"), __FUNCTION__);
		return nullptr;
	}

	const auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);


	if(!IsValid(World))
	{
		UE_LOG(LogGame, Error, TEXT("%S: invalid world"), __FUNCTION__);
		return nullptr;
	}

	auto GameInstance = Cast<UDefconGameInstance>(World->GetGameInstance());

	if(!IsValid(GameInstance))
	{
		UE_LOG(LogGame, Error, TEXT("%S: no game instance available"), __FUNCTION__);
		return nullptr;
	}

	return GameInstance;
}


void UDefconUtils::TransitionToArena(const UObject* WorldContextObject, EDefconArena Arena)
{
	auto GameInstance = GetGameInstance(WorldContextObject);

	if(!IsValid(GameInstance))
	{
		return;
	}

	GameInstance->TransitionToArena(Arena);
}

#endif
