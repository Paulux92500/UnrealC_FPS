// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyWeapon.h"
#include "MyPistoll.generated.h"

/**
 * 
 */
UCLASS()
class FPS_COLLAB_API AMyPistoll : public AMyWeapon
{
	GENERATED_BODY()
public:

	AMyPistoll();

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* SKMesh_gun;
};
