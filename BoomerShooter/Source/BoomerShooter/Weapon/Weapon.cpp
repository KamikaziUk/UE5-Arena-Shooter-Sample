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

#include "Weapon.h"

#include "../Character/BoomerShooterCharacter.h"
#include "../Enemies/EnemyCharacter.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraRaycastOffset = FVector(100.0f, 0.0f, 10.0f);

	auto ParentChild = CreateDefaultSubobject<USceneComponent>(TEXT("Parent"));
	RootComponent = ParentChild;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SkeletalMesh->SetupAttachment(ParentChild);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	WeaponFireLocation = CreateDefaultSubobject<UChildActorComponent>(TEXT("FireLocation"));
	WeaponFireLocation->SetupAttachment(ParentChild);

	RollAmount = 5.0f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();	
	World = GetWorld();
}

void AWeapon::SetFirePressed(bool Pressed)
{
	FirePressed = Pressed;
}

bool AWeapon::IsShooting() const
{
	return CurrentShootTime > 0.0f;
}

bool AWeapon::IsFirePressed() const
{
	return FirePressed;
}

float AWeapon::GetCrosshairAngle() const
{
	return FMath::Lerp(-180.0f, 180.0f, ChangeWeaponTime / 0.3f);
}

float AWeapon::GetCrosshairScale() const
{
	return (1.0f + FMath::Sin(FMath::Clamp(CrosshairHitTime / 0.2f, 0.0f, 1.0f) * PI));
}

void AWeapon::Fire()
{
	if(!IsValid(Character) || !IsValid(Character->GetController()))
	{
		return;
	}

	//if(IsValid(ParticleComponent))
	//{
	//	ParticleComponent->ResetParticles();
	//	ParticleComponent->ForceReset();
	//	ParticleComponent->Activate();
	//}

	if(IsValid(ShootAudio))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShootAudio, GetActorLocation());
	}

	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector SpawnLocation = Character->GetActorLocation() + SpawnRotation.RotateVector(CameraRaycastOffset);

	auto PlayerCam = Character->GetFirstPersonCameraComponent();

	// Raycasting
	for(int i = 0; i < BulletsPerShot; i++)
	{
		float RayLength = 10000.0f;

		auto SpreadDist = (RayLength * SpreadDistance);
		auto RandomRight = FMath::RandRange(-SpreadDist, SpreadDist);
		auto RandomUp = FMath::RandRange(-SpreadDist, SpreadDist);

		FVector RayOrigin = PlayerCam->GetComponentLocation();
		FVector RayEnd = RayOrigin +
			(PlayerCam->GetForwardVector() * RayLength) +
			(PlayerCam->GetRightVector() * RandomRight) +
			(PlayerCam->GetUpVector() * RandomUp);

		if(IsValid(Projectile))
		{
			// Projectile shooting
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			auto ShootDirection = (RayEnd - RayOrigin);
			ShootDirection.Normalize();

			auto Rotation = UKismetMathLibrary::MakeRotFromXZ(ShootDirection, 
				FVector::CrossProduct(ShootDirection, PlayerCam->GetUpVector()));

			auto ProjectileSpawned = World->SpawnActor<AProjectile>(Projectile,
				RayOrigin + (PlayerCam->GetForwardVector() * 100.0f),
				Rotation, ActorSpawnParams);
			ProjectileSpawned->BulletDamage = BulletDamage;
		}
		else
		{
			// Line trace shooting
			FCollisionQueryParams TraceParams(FName(TEXT("")), false, this->GetOwner());
			TraceParams.AddIgnoredActor(Character);

			FCollisionObjectQueryParams CollisionObjectTypes;
			CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
			CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

			FHitResult Hit;
			World->LineTraceSingleByObjectType(
				OUT Hit,
				RayOrigin,
				RayEnd,
				CollisionObjectTypes,
				TraceParams
			);

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			float TraceDistance = RayLength;
			FVector TraceEnd = RayEnd;

			AActor* ActorHit = Hit.GetActor();
			if(IsValid(ActorHit))
			{
				bool HitEnemy = false;
				FVector HitNormal = Hit.ImpactNormal;

				// If we've hit a enemy then damage it
				if(ActorHit->IsA(AEnemyCharacter::StaticClass()))
				{
					auto EnemyCharacter = Cast<AEnemyCharacter>(ActorHit);
					EnemyCharacter->DamageEnemy(BulletDamage);

					CrosshairHitTime = 0.2f;

					HitEnemy = true;
					HitNormal = RayOrigin - RayEnd;
					HitNormal.Normalize();
				}

				TraceDistance = FVector::Distance(WeaponFireLocation->GetComponentLocation(), Hit.ImpactPoint);
				TraceEnd = Hit.ImpactPoint;

				// Spawn VFX (Blood or Ground)
				auto SpawnActor = HitEnemy ? EnemyHit : GroundHit;
				if(IsValid(SpawnActor))
				{
					auto UpDirection = FVector(0, 0, 1);
					if(UKismetMathLibrary::Dot_VectorVector(UpDirection, HitNormal) > 0.9f)
					{
						UpDirection = FVector(0, 1, 0);
					}

					auto Rotation = UKismetMathLibrary::MakeRotFromXZ(HitNormal, FVector::CrossProduct(HitNormal, UpDirection));
					auto SpawnedActor = World->SpawnActor<AActor>(SpawnActor, Hit.ImpactPoint, Rotation, ActorSpawnParams);
				}

				if(!HitEnemy)
				{
					if(IsValid(HitSurfaceAudio))
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSurfaceAudio, TraceEnd);
					}
				}
			}

			// Spawn bullet line VFX
			if(BulletTrace->IsValid())
			{
				auto ShootDirection = TraceEnd - WeaponFireLocation->GetComponentLocation();

				auto UpDirection = FVector(0, 0, 1);
				if(UKismetMathLibrary::Dot_VectorVector(UpDirection, ShootDirection) > 0.9f)
				{
					UpDirection = FVector(0, 1, 0);
				}

				auto Rotation = UKismetMathLibrary::MakeRotFromXZ(ShootDirection, FVector::CrossProduct(ShootDirection, UpDirection));
				UNiagaraComponent* effect =
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						World, BulletTrace, WeaponFireLocation->GetComponentLocation(), Rotation);

				effect->SetNiagaraVariableVec4(FString("BeamEnd"), FVector(TraceDistance, 0, 0));
			}
		}
	}
}

void AWeapon::SpawnWeapon(ABoomerShooterCharacter* TargetCharacter)
{
	Character = TargetCharacter; 

	ChangeWeaponTime = 0.3f;

	if(!IsValid(TargetCharacter))
	{
		return;
	}

	PlayerController = Cast<APlayerController>(Character->GetController());

	// Attach to player
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(
		Cast<USceneComponent>(TargetCharacter->GetFirstPersonCameraComponent()), 
		AttachmentRules, FName(TEXT("GripPoint")));

	SetActorRelativeLocation(FVector(1.0f, 10.3f, -14.3f));
	SetActorRelativeRotation(FRotator(0.0f, 0.0f, -100.0f));
	SetActorRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));

	auto Parent = Character->GetFirstPersonCameraComponent();
	auto RelativeLocation = UKismetMathLibrary::InverseTransformLocation(Parent->GetComponentTransform(), GetActorLocation());
	TargetRelativeLocation = RelativeLocation;
}

void AWeapon::Tick(float DeltaTime)
{
	auto Velocity = FVector(-Character->GetVelocity().X, -Character->GetVelocity().Y, Character->GetVelocity().Z);

	auto Transform = Character->GetFirstPersonCameraComponent()->GetComponentTransform();
	auto RelativeLocation = UKismetMathLibrary::InverseTransformLocation(Transform, GetActorLocation());
	auto RelativeVelocity = UKismetMathLibrary::InverseTransformDirection(Transform, Velocity);

	CurrentSpringVelocity -= RelativeVelocity;

	// Character sway
	// Hooke law spring
	auto SpringTightness = 10.0f;
	auto SpringDamping = 0.2f;
	CurrentSpringVelocity = -(SpringTightness * (RelativeLocation - TargetRelativeLocation)) - (SpringDamping * CurrentSpringVelocity);
	auto NewRelativeLocation = RelativeLocation + (CurrentSpringVelocity * DeltaTime);

	// Clamp so it only moves slightly
	NewRelativeLocation.X = FMath::Clamp(NewRelativeLocation.X, TargetRelativeLocation.X - 1.0f, TargetRelativeLocation.X + 1.0f);
	NewRelativeLocation.Y = FMath::Clamp(NewRelativeLocation.Y, TargetRelativeLocation.Y - 1.0f, TargetRelativeLocation.Y + 1.0f);
	NewRelativeLocation.Z = FMath::Clamp(NewRelativeLocation.Z, TargetRelativeLocation.Z - 1.0f, TargetRelativeLocation.Z + 1.0f);

	auto RollAngle = -FMath::Clamp(RelativeVelocity.Y / Character->GetMoveComponent()->MaxWalkSpeed, -1.0f, 1.0f) * RollAmount;
	SetActorRelativeLocation(NewRelativeLocation);
	SetActorRelativeRotation(FRotator(RollAngle, -90.0f, 0.0f));

	// Shooting
	CurrentShootTime = FMath::Clamp(CurrentShootTime + DeltaTime, 0.0f, ShootTime);
	if(FirePressed)
	{
		if(CurrentShootTime >= ShootTime)
		{
			CurrentShootTime = 0.0f;
			Fire();
		}

		// Minigun rotation
		if (IsValid(SkeletalMesh) && SpinWeapon)
		{
			CurrentRoll = (CurrentRoll + DeltaTime * 700);
			if(CurrentRoll > 360.0f)
			{
				CurrentRoll = 360.0f - CurrentRoll;
			}

			SkeletalMesh->SetRelativeRotation(FRotator(-CurrentRoll, 0.0f, -180.0f));
		}
	}

	ChangeWeaponTime = FMath::Clamp(ChangeWeaponTime - DeltaTime, 0.0f, 0.3f);
	CrosshairHitTime = FMath::Clamp(CrosshairHitTime - DeltaTime, 0.0f, 0.2f);
}