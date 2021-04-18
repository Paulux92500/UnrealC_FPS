// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPistoll.h"

AMyPistoll::AMyPistoll() {
	bIsAutomatic = true;
	fWeaponCooldown = 0;
	idefaultAmmoNumber = 8;
	iAmmos = idefaultAmmoNumber;
	iDefaultMagazinesNumber = 7;
	fReachDistance = 99999;

	FP_Gun = SKMesh_gun;
}