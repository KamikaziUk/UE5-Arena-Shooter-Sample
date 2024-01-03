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

#include "BoomerShooterCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ABoomerShooterCharacter::ABoomerShooterCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	CameraRollAmount = 1.0f;
	CameraBobSpeed = 0.01f;
	CameraBobHeight = 20.0f;
	CameraBobFallOff = 10.0f;
}

void ABoomerShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	CanDash = true;
	MovementVector = FVector2D(0, 0);
	Health = MaxHealth;

	if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Slow on performance but using for demonstration
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnerManager::StaticClass(), FoundActors);
	if(FoundActors.Num() == 1)
	{
		SpawnerManager = Cast<AEnemySpawnerManager>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find enemy spawn manager"));
	}

	CurrentWeapon = nullptr;
	UpdateWeapon(1);

	auto MovementComponent = FindComponentByClass(UCharacterMovementComponent::StaticClass());
	if(IsValid(MovementComponent))
	{
		CharacterMovement = Cast<UCharacterMovementComponent>(MovementComponent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Character Movement Component Found on Player"));
	}

	CameraRelativeLocation = FirstPersonCameraComponent->GetRelativeLocation();
}

void ABoomerShooterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABoomerShooterCharacter::JumpInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::Look);

		//Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::Dash);

		//Change Weapon
		EnhancedInputComponent->BindAction(ChangeWeaponSlot0, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::ChangeWeapon1);
		EnhancedInputComponent->BindAction(ChangeWeaponSlot1, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::ChangeWeapon2);
		EnhancedInputComponent->BindAction(ChangeWeaponSlot2, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::ChangeWeapon3);
		EnhancedInputComponent->BindAction(ChangeWeaponSlot3, ETriggerEvent::Triggered, this, &ABoomerShooterCharacter::ChangeWeapon4);

		//Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ABoomerShooterCharacter::FireInputStart);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Canceled, this, &ABoomerShooterCharacter::FireInputCancelled);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ABoomerShooterCharacter::FireInputCancelled);
	}
}

void ABoomerShooterCharacter::Move(const FInputActionValue& Value)
{
	MovementVector = Value.Get<FVector2D>();

	if(IsValid(Controller))
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABoomerShooterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(IsValid(Controller))
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABoomerShooterCharacter::JumpInput(const FInputActionValue& Value)
{
	if(this->CanJump())
	{
		if(IsValid(JumpAudio))
		{
			UGameplayStatics::PlaySoundAtLocation(this, JumpAudio, GetActorLocation());
		}
	}

	this->Jump();
}

void ABoomerShooterCharacter::Dash(const FInputActionValue& Value)
{
	if(CanDash)
	{
		auto CharacterBase = Cast<ACharacter>(this);

		auto LaunchDir = this->GetActorForwardVector();
		if(MovementVector.Length() > 0.1f)
		{
			LaunchDir = (this->GetActorForwardVector() * MovementVector.Y) +
				(this->GetActorRightVector() * MovementVector.X);
		}

		CharacterBase->LaunchCharacter(LaunchDir * DashVelocity, false, false);
		CanDash = false;

		if(IsValid(DashAudio))
		{
			UGameplayStatics::PlaySoundAtLocation(this, DashAudio, GetActorLocation());
		}
	}
}

void ABoomerShooterCharacter::ChangeWeapon1(const FInputActionValue& Value)
{
	UpdateWeapon(0);
}

void ABoomerShooterCharacter::ChangeWeapon2(const FInputActionValue& Value)
{
	UpdateWeapon(1);
}

void ABoomerShooterCharacter::ChangeWeapon3(const FInputActionValue& Value)
{
	UpdateWeapon(2);
}

void ABoomerShooterCharacter::ChangeWeapon4(const FInputActionValue& Value)
{
	UpdateWeapon(3);
}

void ABoomerShooterCharacter::FireInputStart()
{
	if(IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetFirePressed(true);
	}
}

void ABoomerShooterCharacter::FireInputCancelled()
{
	if(IsValid(CurrentWeapon))
	{
		CurrentWeapon->SetFirePressed(false);
	}
}

void ABoomerShooterCharacter::UpdateWeapon(int WeaponId)
{
	if(IsValid(World))
	{
		if(WeaponId < 0 || WeaponId >= Weapons.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid WeaponId %d"), WeaponId);
			return;
		}

		const FRotator SpawnRotation = FirstPersonCameraComponent->GetComponentRotation();
		const FVector SpawnLocation = FirstPersonCameraComponent->GetComponentLocation();

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if(IsValid(CurrentWeapon))
		{
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}

		CurrentWeapon = World->SpawnActor<AWeapon>(Weapons[WeaponId], SpawnLocation, SpawnRotation, ActorSpawnParams);
		CurrentWeapon->SpawnWeapon(this);
	}
}

bool ABoomerShooterCharacter::IsWeaponShooting()
{
	if(IsValid(CurrentWeapon))
	{
		return CurrentWeapon->IsShooting();
	}

	return false;
}

void ABoomerShooterCharacter::Landed(const FHitResult& Hit)
{
	CanDash = true;

	if(IsValid(JumpLandAudio))
	{
		UGameplayStatics::PlaySoundAtLocation(this, JumpLandAudio, GetActorLocation());
	}
}

void ABoomerShooterCharacter::Tick(float DeltaTime)
{
	const float Pi = 3.1415926535897932f;

	LevelTimer += DeltaTime;

	auto Velocity = FVector(-GetVelocity().X, -GetVelocity().Y, GetVelocity().Z);
	auto Transform = FirstPersonCameraComponent->GetRelativeTransform();
	auto RelativeVelocity = UKismetMathLibrary::InverseTransformDirection(Transform, Velocity);

	// Camera roll
	auto RollAngle = -FMath::Clamp(RelativeVelocity.Y / CharacterMovement->MaxWalkSpeed, -1.0f, 1.0f) * CameraRollAmount;
	auto RelativeRotation = GetControlRotation();
	RelativeRotation.Roll = RollAngle;
	FirstPersonCameraComponent->SetWorldRotation(RelativeRotation);

	// Camera bobbing up/down
	BobTime += (Velocity.Size() * CameraBobSpeed * DeltaTime);

	// Loop back around so we don't get floating point issues
	if(BobTime >= Pi)
	{
		BobTime = BobTime - Pi;
	}

	// Decrease back to normal when not moving or in air
	if(Velocity.Size() < 5.0f || !CharacterMovement->IsMovingOnGround())
	{
		BobTime = FMath::Max(BobTime - (CameraBobFallOff * DeltaTime), 0.0f);
	}

	BobVelocity = (0.5f + 0.5f * FMath::Sin(BobTime)) * CameraBobHeight;
	FirstPersonCameraComponent->SetRelativeLocation(CameraRelativeLocation + FVector(0.0f, 0.0f, BobVelocity));

	// Decrease damage indicator times
	for(int i = 0; i < 4; i++)
	{
		DamageDirectionUI[i] = FMath::Max(0, DamageDirectionUI[i] - DeltaTime);
	}
}

// Blueprint functions
void ABoomerShooterCharacter::KilledEnemy(int ScoreIncrease)
{
	Combo += 1;
	Score += (float)ScoreIncrease * (float)Combo;
}

void ABoomerShooterCharacter::TookDamage(int Damage, FVector DamageLocation)
{
	Combo = 0;
	Health -= Damage;

	if(IsValid(TakeDamageAudio))
	{
		UGameplayStatics::PlaySoundAtLocation(this, TakeDamageAudio, GetActorLocation());
	}

	auto DirectionToTarget = DamageLocation - GetActorLocation();

	auto ForwardDot = FVector::DotProduct(FirstPersonCameraComponent->GetForwardVector(), DirectionToTarget);
	auto RightDot = FVector::DotProduct(FirstPersonCameraComponent->GetRightVector(), DirectionToTarget);

	if(FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		if(ForwardDot > 0)
		{
			DamageDirectionUI[0] = DamageDirectionUIFadeTime;
		}
		else
		{
			DamageDirectionUI[2] = DamageDirectionUIFadeTime;
		}
	}
	else
	{
		if(RightDot > 0)
		{
			DamageDirectionUI[1] = DamageDirectionUIFadeTime;
		}
		else
		{
			DamageDirectionUI[3] = DamageDirectionUIFadeTime;
		}
	}

	if(Health <= 0)
	{
		UGameplayStatics::OpenLevel(World, LevelToLoadOnDeath);
	}
}

float ABoomerShooterCharacter::GetDamageDirectionUI(int DirectionId)
{
	return FMath::Lerp(0.0f, 1.0f, DamageDirectionUI[DirectionId] / DamageDirectionUIFadeTime);
}

float ABoomerShooterCharacter::GetHealth01()
{
	return (float)Health / (float)MaxHealth;
}

float ABoomerShooterCharacter::GetLevelTimer()
{
	return LevelTimer;
}

FString ABoomerShooterCharacter::GetComboText()
{
	auto ComboStr = FString::FromInt(Combo);
	return ComboStr.Append(TEXT("X"));
}

FString ABoomerShooterCharacter::GetScoreText()
{
	return FString::FormatAsNumber(Score);
}

const AWeapon* ABoomerShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}