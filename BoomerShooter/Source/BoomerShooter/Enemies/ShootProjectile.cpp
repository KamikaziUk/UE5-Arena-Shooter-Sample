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

#include "ShootProjectile.h"

#include "../Weapon/Projectile.h"

#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"

UShootProjectile::UShootProjectile()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentTime = {};
	CurrentShootTimer = {};
	World = nullptr;
}

void UShootProjectile::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
}

void UShootProjectile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(IsValid(World))
	{
		CurrentTime -= DeltaTime;
		CurrentShootTimer += DeltaTime;

		// Shooting
		if(CurrentShootTimer >= ShootTime && IsValid(Projectile))
		{
			CurrentShootTimer = 0.0f;

			// Projectile shooting
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			auto ProjectileSpawned = World->SpawnActor<AProjectile>(Projectile,
				GetComponentLocation(),
				GetComponentRotation(), ActorSpawnParams);
			ProjectileSpawned->BulletDamage = Damage;

			if(IsValid(ShootAudio))
			{
				UGameplayStatics::PlaySoundAtLocation(this, ShootAudio, GetComponentLocation());
			}
		}
	}
}