#include "MyWeapon.h"
#include "Camera/CameraComponent.h"
#include "MyEnnemy.h"
#include "DrawDebugHelpers.h"
#include "FPS_CollabGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
}

void AMyWeapon::WeaponReload()
{
	iAmmos = idefaultAmmoNumber;
}

void AMyWeapon::WeaponShoot(const FVector& origin, UCameraComponent* camera, const FRotator& rotation, const FVector& GunOffset, UParticleSystem* muzzleParticuleSystem, UParticleSystem* impactParticuleSystem, USceneComponent* FP_MuzzleLocation)
{
	//// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + rotation.RotateVector(GunOffset);


	FVector StarLocation = camera->GetComponentLocation() + camera->GetForwardVector();

	FVector EndLocation = StarLocation + (camera->GetForwardVector() * fReachDistance);

	FHitResult hit;

	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(this->GetOwner());

	bool isHit = GetWorld()->LineTraceSingleByChannel(hit, StarLocation, EndLocation, ECC_Visibility, CollisionParam);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleParticuleSystem, SpawnLocation, rotation, true);

	AActor* ActorHit = hit.GetActor();

	if (ActorHit) {

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impactParticuleSystem, hit.Location, rotation, true);

		if (hit.GetActor()->IsA(AMyEnnemy::StaticClass()) && ((AMyEnnemy*)ActorHit)->BA_TakeDamage(10)) {

			AFPS_CollabGameMode* mymode = Cast<AFPS_CollabGameMode>(GetWorld()->GetAuthGameMode());
			mymode->deleteEnnemy((AMyEnnemy*)hit.GetActor());
			ActorHit->Destroy();
		}
	}

	// Remove the bullet that was shot
	--iAmmos;
	UE_LOG(LogTemp, Warning, TEXT("%d"), iAmmos);
}

