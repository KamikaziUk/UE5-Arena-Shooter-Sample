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

#include "../Weapon/Weapon.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BoomerShooterCharacter.generated.h"

class AEnemySpawnerManager;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ABoomerShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TArray<TSubclassOf<class AWeapon>> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float DashVelocity;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float DamageDirectionUIFadeTime;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraRollAmount;
	
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobHeight;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobFallOff;

public:
	ABoomerShooterCharacter();
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	AEnemySpawnerManager* GetSpawnManager() const { return SpawnerManager; }
	UCharacterMovementComponent* GetMoveComponent() const { return CharacterMovement; }

	// Blueprint functions
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void KilledEnemy(int ScoreIncrease);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void TookDamage(int Damage, FVector DamageLocation);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	float GetDamageDirectionUI(int DirectionId);

	UFUNCTION(BlueprintCallable, Category = Player)
	float GetHealth01();

	UFUNCTION(BlueprintCallable, Category = Player)
	float GetLevelTimer();

	UFUNCTION(BlueprintCallable, Category = Player)
	FString GetComboText();

	UFUNCTION(BlueprintCallable, Category = Player)
	FString GetScoreText();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	const AWeapon* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool IsWeaponShooting();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void ChangeWeapon1(const FInputActionValue& Value);
	void ChangeWeapon2(const FInputActionValue& Value);
	void ChangeWeapon3(const FInputActionValue& Value);
	void ChangeWeapon4(const FInputActionValue& Value);
	void FireInputStart();
	void FireInputCancelled();
	void UpdateWeapon(int WeaponId);

	FVector CameraRelativeLocation;
	float BobVelocity;
	float BobTime;
	bool CanDash;
	float DamageDirectionUI[4];
	FVector2D MovementVector;

	float LevelTimer;
	int Score;
	int Combo;
	int Health;

	AEnemySpawnerManager* SpawnerManager;
	UCharacterMovementComponent* CharacterMovement;
	AWeapon* CurrentWeapon;
	UWorld* World;
};