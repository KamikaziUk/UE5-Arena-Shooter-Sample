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
#include "Sound/SoundBase.h"

#include "BoomerShooterCharacter.generated.h"

class AEnemySpawnerManager;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerInteractionPressedDelegate);

UCLASS(config=Game)
class ABoomerShooterCharacter : public ACharacter
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Loading)
	FName LevelToLoadOnDeath = {};

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TArray<TSubclassOf<class AWeapon>> Weapons = {};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractionAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot0 = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot1 = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot2 = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponSlot3 = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* JumpAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* JumpLandAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* DashAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* TakeDamageAudio = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float DashVelocity = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int MaxHealth = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float DamageDirectionUIFadeTime = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraRollAmount = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobSpeed = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobHeight = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CameraBobFallOff = 0;

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

	UPROPERTY(BlueprintAssignable, Category = Events)
	FPlayerInteractionPressedDelegate OnPlayerInteractionPress;

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;
	void JumpInput(const FInputActionValue& Value);

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void Interaction(const FInputActionValue& Value);
	void ChangeWeapon1(const FInputActionValue& Value);
	void ChangeWeapon2(const FInputActionValue& Value);
	void ChangeWeapon3(const FInputActionValue& Value);
	void ChangeWeapon4(const FInputActionValue& Value);
	void FireInputStart();
	void FireInputCancelled();
	void UpdateWeapon(int WeaponId);

	FVector CameraRelativeLocation = {};
	float BobVelocity = 0;
	float BobTime = 0;
	bool CanDash = false;
	float DamageDirectionUI[4] = {};
	FVector2D MovementVector = {};

	float LevelTimer = 0;
	int Score = 0;
	int Combo = 0;
	int Health = 0;

	TObjectPtr<AEnemySpawnerManager> SpawnerManager = nullptr;
	TObjectPtr<UCharacterMovementComponent> CharacterMovement = nullptr;
	TObjectPtr<AWeapon> CurrentWeapon = nullptr;
	TObjectPtr<UWorld> World = nullptr;
};