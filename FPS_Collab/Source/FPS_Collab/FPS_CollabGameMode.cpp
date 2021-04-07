// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_CollabGameMode.h"
#include "FPS_CollabHUD.h"
#include "FPS_CollabCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_CollabGameMode::AFPS_CollabGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_CollabHUD::StaticClass();
}
