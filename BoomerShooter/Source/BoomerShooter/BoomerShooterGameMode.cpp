// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright @ MGGames 2023

#include "BoomerShooterGameMode.h"
#include "Character/BoomerShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABoomerShooterGameMode::ABoomerShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
