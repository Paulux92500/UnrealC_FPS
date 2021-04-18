// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnnemy.h"
#include "FPS_CollabCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "AIController.h"

// Sets default values
AMyEnnemy::AMyEnnemy(): f_life(30)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMyEnnemy::MoveToTarget(AFPS_CollabCharacter* Target)
{
    if (pController)
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);

        // (Stopping distance in unity)
        MoveRequest.SetAcceptanceRadius(25.0f);

        FNavPathSharedPtr NavPath;

        pController->MoveTo(MoveRequest, &NavPath);
    }
}

bool AMyEnnemy::BA_TakeDamage(float damage)
{
    f_life -= damage;
    return  f_life <= 0;
}

// Called when the game starts or when spawned
void AMyEnnemy::BeginPlay()
{
	Super::BeginPlay();
	pController = Cast<AAIController>(GetController());
	character = Cast<AFPS_CollabCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void AMyEnnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveToTarget(character);
}

// Called to bind functionality to input
void AMyEnnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

