// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyWeapon.h"
#include "MyAssaultRifle.generated.h"

/**
 * 
 */
UCLASS()
class FPS_COLLAB_API AMyAssaultRifle : public AMyWeapon
{
	GENERATED_BODY()

public:
	AMyAssaultRifle();

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* SKMesh_gun;
};
