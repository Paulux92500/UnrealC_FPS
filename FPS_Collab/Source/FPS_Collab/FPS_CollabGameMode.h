// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnerEnnemy.h"
#include "MyEnnemy.h"
#include "GameFramework/GameModeBase.h"
#include "FPS_CollabGameMode.generated.h"

UCLASS(minimalapi)
class AFPS_CollabGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPS_CollabGameMode();

	void BeginPlay() override;
	virtual void Tick(float deltaTime);

	void FindAllActors(UWorld* World, TArray<ASpawnerEnnemy*>& Out);
	TArray<ASpawnerEnnemy*> FoundActorsSpawner;
	TArray<int> WavesSpawn = { 3, 6, 9, 12 };
	TArray<AMyEnnemy*> EnnemysInLevel;
	int currentWave = 0;
	float fDelayTimer;
	float fDelayCooldown = 1;

	void deleteEnnemy(AMyEnnemy* ennemyToDelete);

	void spawningEnnemy();

	UPROPERTY(EditDefaultsOnly, Category = Ennemy)
		TSubclassOf<class AMyEnnemy> Ennemy;
};



