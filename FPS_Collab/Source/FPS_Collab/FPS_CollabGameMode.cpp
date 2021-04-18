// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_CollabGameMode.h"
#include "FPS_CollabHUD.h"
#include "MyEnnemy.h"
#include "FPS_CollabCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

AFPS_CollabGameMode::AFPS_CollabGameMode()
	: Super()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_CollabHUD::StaticClass();
}

void AFPS_CollabGameMode::BeginPlay()
{
	FindAllActors(GetWorld(), FoundActorsSpawner);
	fDelayTimer = GetGameTimeSinceCreation();
}

void AFPS_CollabGameMode::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (WavesSpawn[currentWave] > 0)
	{
		if (fDelayTimer < GetGameTimeSinceCreation()) {
			spawningEnnemy();
			fDelayTimer = GetGameTimeSinceCreation() + fDelayCooldown;
		}
	}
}

void AFPS_CollabGameMode::FindAllActors(UWorld* World, TArray<ASpawnerEnnemy*>& Out)
{
	for (TActorIterator<ASpawnerEnnemy> It(World); It; ++It)
	{
		Out.Add(*It);
	}
}

void AFPS_CollabGameMode::deleteEnnemy(AMyEnnemy* ennemyToDelete)
{
	for (AMyEnnemy* ennemy : EnnemysInLevel)
	{
		if (ennemyToDelete == ennemy)
		{
			EnnemysInLevel.Remove(ennemy);
		}
	}

	if (EnnemysInLevel.Num() <= 0 && WavesSpawn[currentWave] == 0)
	{
		if (currentWave < WavesSpawn.Num() - 1)
		{
			currentWave++;
			spawningEnnemy();
		}
	}
}

void AFPS_CollabGameMode::spawningEnnemy()
{
	UWorld* world = GetWorld();
	if (world)
	{
		if (Ennemy)
		{
			FActorSpawnParameters spawnParam;
			spawnParam.Owner = this;
			FRotator rotator;

			int aleaInt = FMath::RandRange(0, FoundActorsSpawner.Num() - 1);
			FVector spawnLocation = FoundActorsSpawner[aleaInt]->GetActorLocation();
			EnnemysInLevel.Add(world->SpawnActor<AMyEnnemy>(Ennemy, spawnLocation, rotator, spawnParam));
			WavesSpawn[currentWave] -= 1;
		}
	}
}
