// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright (c) Marty Green 2023
// https://github.com/KamikaziUk

// MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "CoreMinimal.h"

#include "Projectile.h"

#include "GameFramework/Actor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"

#include "Weapon.generated.h"

class ABoomerShooterCharacter;

UCLASS()
class BOOMERSHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()

	// Default components
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	USkeletalMeshComponent* SkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	UChildActorComponent* WeaponFireLocation = nullptr;

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AActor> GroundHit = {};

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AActor> EnemyHit = {};

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AProjectile> Projectile = {};

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* ShootAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* HitSurfaceAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class UNiagaraSystem* BulletTrace = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	bool SpinWeapon = false;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float ShootTime = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int BulletsPerShot = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float SpreadDistance = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int BulletDamage = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float RollAmount = 0;
	
public:	
	AWeapon();

	void SetFirePressed(bool Pressed);

	// Blueprint functions
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SpawnWeapon(ABoomerShooterCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsShooting() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	float GetCrosshairAngle() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	float GetCrosshairScale() const;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:
	void Fire();
	bool IsFirePressed() const;

	TObjectPtr<ABoomerShooterCharacter> Character = nullptr;
	TObjectPtr<UWorld> World = nullptr;
	TObjectPtr<APlayerController> PlayerController = nullptr;

	FVector TargetRelativeLocation = {};
	float CurrentShootTime = 0;
	FVector CurrentSpringVelocity = {};
	bool FirePressed = false;
	float CurrentRoll = 0;
	float CrosshairHitTime = 0;
	float ChangeWeaponTime = 0;
	FVector CameraRaycastOffset = {};
};
