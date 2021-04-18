// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAssaultRifle.h"

AMyAssaultRifle::AMyAssaultRifle() {
	bIsAutomatic = true;
	fWeaponCooldown = 0.05;
	idefaultAmmoNumber = 34;
	iAmmos = idefaultAmmoNumber;
	iDefaultMagazinesNumber = 22;
	fReachDistance = 99999;

	FP_Gun = SKMesh_gun;
}