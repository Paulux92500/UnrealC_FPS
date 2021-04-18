// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyWeapon.generated.h"

class UCameraComponent;
class USceneComponent;
class USkeletalMeshComponent;


UCLASS()
class FPS_COLLAB_API AMyWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyWeapon();

	void WeaponReload();

	void WeaponShoot(const FVector& origin, UCameraComponent* camera, const FRotator& rotation, const FVector& GunOffset, UParticleSystem* muzzleParticuleSystem, UParticleSystem* impactParticuleSystem, USceneComponent* FP_MuzzleLocation);

	bool HasEnoughBullet() { return iAmmos > 0; }

	bool bIsAutomatic;
	float fDelay = 0.3f;
	float fWeaponCooldown;


protected:
	int iAmmos;
	int idefaultAmmoNumber;
	int iDefaultMagazinesNumber;
	float fReachDistance;

	//USceneComponent* FP_MuzzleLocation;
	USkeletalMeshComponent* FP_Gun;


	//virtual void WeaponShoot(const FVector& start, const FVector& end, UParticuleSystem muzzleParticuleSystem) PURE_VIRTUAL(AMyWeapon::WeaponShoot);

};
