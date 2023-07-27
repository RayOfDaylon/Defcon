// Fill out your copyright notice in the Description page of Project Settings.

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
	FireSmartbomb
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


