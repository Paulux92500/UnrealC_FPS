// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_CollabCharacter.h"
#include "FPS_CollabProjectile.h"
#include "Animation/AnimInstance.h"
#include "MyWeapon.h"
#include "MyPistoll.h"
#include "MyAssaultRifle.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPS_CollabCharacter

AFPS_CollabCharacter::AFPS_CollabCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	// Create a gun mesh component
	FP_AR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_AR"));
	FP_AR->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_AR->bCastDynamicShadow = false;
	FP_AR->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_AR->SetupAttachment(RootComponent);
	FP_AR->SetHiddenInGame(false);
	//FP_AR->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AFPS_CollabCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_AR->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	// Adds the weapons
	UWorld* const World = GetWorld();


	if (World != nullptr) {

		const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
		const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();

		_weapons.Push(World->SpawnActor<AMyPistoll>(_firstWeapon, SpawnLocation, SpawnRotation));
		_weapons.Push(World->SpawnActor<AMyAssaultRifle>(_secondWeapon, SpawnLocation, SpawnRotation));
	}

	_currentWeaponIndex = 0;
	fWeaponCooldown = _weapons[1]->fWeaponCooldown;

	FP_AR->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	FP_Gun->SetHiddenInGame(true);
}

//////////////////////////////////////////////////////////////////////////
// Input


void AFPS_CollabCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanShootAuto && _currentWeaponIndex == 1 && _weapons[_currentWeaponIndex]->HasEnoughBullet()) {

		if (fWeaponTimer < GetGameTimeSinceCreation()) {
			UE_LOG(LogTemp, Warning, TEXT("AR"));
			OnFire();

			fWeaponTimer += fWeaponCooldown;
		}

	}
}

void AFPS_CollabCharacter::ReloadWeapon() {
	_weapons[_currentWeaponIndex]->WeaponReload();
}

void AFPS_CollabCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPS_CollabCharacter::WeaponTypeShoot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPS_CollabCharacter::ResetAutoShoot);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPS_CollabCharacter::ReloadWeapon);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFPS_CollabCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPS_CollabCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPS_CollabCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPS_CollabCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPS_CollabCharacter::LookUpAtRate);



	// Switch weapon input
	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &AFPS_CollabCharacter::SwitchWeapon);
}

void AFPS_CollabCharacter::SetAutoShoot() {

	if (_currentWeaponIndex == 1) {
		bCanShootAuto = !bCanShootAuto;
		fWeaponTimer = GetGameTimeSinceCreation();
	}

}

void AFPS_CollabCharacter::ResetAutoShoot() {

	if (_currentWeaponIndex == 1) {
		bCanShootAuto = false;
	}

}

void AFPS_CollabCharacter::OnFire()
{
	
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();

			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();

				FVector StartLocation = FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector();

				FVector LineTraceEnd = StartLocation + FirstPersonCameraComponent->GetForwardVector();

				_weapons[_currentWeaponIndex]->WeaponShoot(StartLocation, FirstPersonCameraComponent, SpawnRotation, GunOffset, muzzleParticuleSystem, impactParticuleSystem, FP_MuzzleLocation);
			}
		}
	

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AFPS_CollabCharacter::SwitchWeapon() {

	if (_currentWeaponIndex == 0) {
		_currentWeaponIndex = 1;
		FP_Gun->SetHiddenInGame(false);
		FP_AR->SetHiddenInGame(true);
	}
	else {
		_currentWeaponIndex = 0;
		FP_Gun->SetHiddenInGame(true);
		FP_AR->SetHiddenInGame(false);
	}
}


void AFPS_CollabCharacter::WeaponTypeShoot()
{

	if (_currentWeaponIndex == 0 && _weapons[_currentWeaponIndex]->HasEnoughBullet()) {
		OnFire();
		UE_LOG(LogTemp, Warning, TEXT("Pistol"));
	}
	else if (_currentWeaponIndex == 1) {
		UE_LOG(LogTemp, Warning, TEXT("AR"));
		SetAutoShoot();
	}


}

void AFPS_CollabCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFPS_CollabCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPS_CollabCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}



void AFPS_CollabCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPS_CollabCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPS_CollabCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPS_CollabCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AFPS_CollabCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPS_CollabCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPS_CollabCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPS_Project19Character::TouchUpdate);
		return true;
	}

	return false;
}
