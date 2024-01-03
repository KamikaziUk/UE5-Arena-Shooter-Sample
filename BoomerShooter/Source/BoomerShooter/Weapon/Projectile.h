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

#include "../Enemies/EnemySpawnerManager.h"

#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"

#include "Projectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Bounce        UMETA(DisplayName = "Bounce"),
	Rocket        UMETA(DisplayName = "Rocket"),
	Standard      UMETA(DisplayName = "Standard"),
};

UCLASS()
class BOOMERSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
	// Default component
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float Speed;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float CollisionSize;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	bool EnemyBullet;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AActor> Explosion;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundBase* ExplosionAudio;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class AActor> EnemyHit;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	EProjectileType ProjectileType;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float DamageRadius;

public:	
	AProjectile();

	int BulletDamage;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() 
	void OnComponentHit(
		UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION() 
	void OnComponentOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	FVector PreviousLocation;
	AEnemySpawnerManager* SpawnerManager;
	UWorld* World;
	AActor* PlayerActor;
	ABoomerShooterCharacter* PlayerCharacter;
};
