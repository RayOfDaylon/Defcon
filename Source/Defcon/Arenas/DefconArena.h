// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefconArena.generated.h"


UENUM()
enum class EDefconArena : uint8
{
	Intro,
	MainMenu,
	MissionPicker,
	Prewave,
	Play,
	Postwave,
	GameOver,
	Help,
	Credits
};

/*
	Arena (view) transition graph:

	<< Intro

	Intro          -> (Enter) -> MainMenu

	MainMenu       -> (Start)   -> MissionPicker
	               -> (Help)    -> Help    -> MainMenu
			       -> (Info)    -> Credits -> MainMenu
				   >> (Exit)

	MissionPicker  -> (Select)  -> Prewave
	MissionPicker  -> (Esc)     -> MainMenu

	Prewave        -> Play
	Prewave        -> (Esc)    -> MissionPicker

	Play           -> PostWave
	Play           -> (Esc)    -> MissionPicker

	Postwave       -> Prewave
	Postwave       -> GameOver -> MainMenu
	Postwave       -> (Esc)    -> MissionPicker
*/


UENUM()
enum class EDefconPawnNavigationEvent : uint8
{
	Up,
	Down,
	Left,     // Used if auto-reverse desired
	Right,    // Used if auto-reverse desired
	Reverse,  // used if explicit reverse button is enabled
	Hyperspace
};


UENUM()
enum class EDefconPawnWeaponEvent : uint8
{
	FireLaser,
	DetonateSmartbomb
};


// todo: move to plugin
UENUM()
enum class ENavigationKey : uint8
{
	Up,
	Down,
	Left,
	Right
};


