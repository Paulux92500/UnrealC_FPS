// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnnemy.generated.h"

class AFPS_CollabCharacter;

UCLASS()
class FPS_COLLAB_API AMyEnnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnnemy();

	AFPS_CollabCharacter* character;

	void MoveToTarget(AFPS_CollabCharacter* Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	float f_life;

	bool BA_TakeDamage(float damage);

	class AAIController* pController;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
