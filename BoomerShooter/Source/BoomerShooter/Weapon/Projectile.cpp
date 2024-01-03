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

#include "Projectile.h"

#include "../Character/BoomerShooterCharacter.h"
#include "../Enemies/EnemyCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
 	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();	

	World = GetWorld();
	PlayerActor = Cast<AActor>(World->GetFirstPlayerController()->GetPawn());
	PlayerCharacter = Cast<ABoomerShooterCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));

	Mesh->OnComponentHit.AddDynamic(this, &AProjectile::OnComponentHit);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnComponentOverlap);

	PreviousLocation = Mesh->GetComponentLocation();
	SpawnerManager = PlayerCharacter->GetSpawnManager();
}

void AProjectile::OnComponentHit(
	UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AProjectile::OnComponentOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Mesh->SetPhysicsLinearVelocity(Mesh->GetForwardVector() * Speed * DeltaTime);

	if(IsValid(World) && IsValid(PlayerActor))
	{
		FVector RayOrigin = Mesh->GetComponentLocation();
		FVector RayEnd = PreviousLocation;

		FCollisionQueryParams TraceParams(FName(TEXT("")), false, this->GetOwner());

		auto EnemyActors = SpawnerManager->GetWaveCharacters();

		if(EnemyBullet)
		{	
			// Add all the enemies actors, so the player doesn't hit them
			for(int i = 0; i < EnemyActors.Num(); i++)
			{
				if(IsValid(EnemyActors[i]))
				{
					TraceParams.AddIgnoredActor(EnemyActors[i]);
				}
			}
		}
		else
		{
			TraceParams.AddIgnoredActor(PlayerActor);
		}

		FCollisionObjectQueryParams CollisionObjectTypes;
		CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);

		// Sweep
		FHitResult Hit;

		if(ProjectileType == EProjectileType::Rocket)
		{
			FCollisionShape SweepCollision = FCollisionShape::MakeSphere(CollisionSize);

			// Sweep from last to current position, as it moves fast we can't use overlap events
			World->SweepSingleByObjectType(
				OUT Hit,
				RayOrigin,
				RayEnd,
				FQuat::Identity,
				CollisionObjectTypes,
				SweepCollision,
				TraceParams
			);
		}
		else if(ProjectileType == EProjectileType::Bounce)
		{
			// Change to line trace if we can find out the impact normal?
			FCollisionShape SweepCollision = FCollisionShape::MakeSphere(2.0f);

			World->SweepSingleByObjectType(
				OUT Hit,
				RayOrigin,
				RayEnd,
				FQuat::Identity,
				CollisionObjectTypes,
				SweepCollision,
				TraceParams
			);
		}

		AActor* ActorHit = Hit.GetActor();

		if(IsValid(ActorHit))
		{
			if(ProjectileType == EProjectileType::Rocket)
			{
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				if(IsValid(Explosion))
				{
					auto ExplosionActor = World->SpawnActor<AActor>(Explosion,
						ActorHit->GetActorLocation(), ActorHit->GetActorRotation(), ActorSpawnParams);
					ExplosionActor->SetActorLocationAndRotation(Hit.ImpactPoint, FQuat::Identity);

					if(IsValid(ExplosionAudio))
					{
						UGameplayStatics::PlaySoundAtLocation(this, ExplosionAudio, GetActorLocation());
					}
				}

				if(EnemyBullet)
				{	
					if(IsValid(PlayerCharacter) && ActorHit == PlayerCharacter)
					{
						PlayerCharacter->TookDamage(BulletDamage, GetActorLocation());
					}
				}
				else
				{
					auto UpDirection = FVector(0, 0, 1);
					if(UKismetMathLibrary::Dot_VectorVector(UpDirection, Hit.ImpactNormal) > 0.9f)
					{
						UpDirection = FVector(0, 1, 0);
					}

					auto Rotation = UKismetMathLibrary::MakeRotFromXZ(Hit.ImpactNormal, FVector::CrossProduct(Hit.ImpactNormal, UpDirection));

					for(int i = 0; i < EnemyActors.Num(); i++)
					{
						if(IsValid(EnemyActors[i]))
						{
							auto ActorPos = EnemyActors[i]->GetActorLocation();

							if(FVector::Distance(Hit.ImpactPoint, ActorPos) <= DamageRadius)
							{
								auto EnemyCharacter = EnemyActors[i];
								EnemyCharacter->DamageEnemy(BulletDamage);

								if(IsValid(EnemyHit))
								{
									auto SpawnedActor = World->SpawnActor<AActor>(EnemyHit,
										EnemyActors[i]->GetActorLocation(), EnemyActors[i]->GetActorRotation(), ActorSpawnParams);
									SpawnedActor->SetActorLocationAndRotation(ActorPos, Rotation);
								}
							}
						}
					}
				}

				Destroy();
			}
			else if(ProjectileType == EProjectileType::Bounce)
			{
				/*
				auto impactNormal = Hit.ImpactNormal + Hit.ImpactPoint;

				//DrawDebugLine(World,
				//	Mesh->GetComponentLocation(),
				//	Mesh->GetComponentLocation() + (Hit.ImpactNormal * 100), FColor(0, 255, 0), false, 10.0f);

				//DrawDebugLine(World,
				//	Mesh->GetComponentLocation(),
				//	Mesh->GetComponentLocation() + (prevNormal * 100), FColor(255, 0, 0), false, 10.0f);

				auto reflectDir = FMath::GetReflectionVector(Mesh->GetForwardVector(), Hit.ImpactNormal);

				//DrawDebugLine(World,
				//	Mesh->GetComponentLocation(),
				//	Mesh->GetComponentLocation() + (reflectDir * 100), FColor(255, 255, 255), false, 10.0f);

				auto newDir = UKismetMathLibrary::MakeRotFromXZ(reflectDir, Mesh->GetUpVector());
				Mesh->SetWorldRotation(newDir);

				auto dotTest = FMath::Abs(FVector::DotProduct(Mesh->GetForwardVector(), Hit.ImpactNormal));
				auto dot2 = FMath::Lerp(30.0f, 55.0f, dotTest);
				Mesh->SetWorldLocation(Hit.ImpactPoint + (Hit.ImpactNormal * 10.0f));
				*/
			}
		}
	}

	PreviousLocation = Mesh->GetComponentLocation();
	SetActorLocation(Mesh->GetComponentLocation());
	SetActorRotation(Mesh->GetComponentRotation());
}

