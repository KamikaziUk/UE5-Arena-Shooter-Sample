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

#include "DamageZone.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADamageZone::ADamageZone()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	BoxCollider->SetCanEverAffectNavigation(false);
	BoxCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RootComponent = BoxCollider;
}

void ADamageZone::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ADamageZone::OnComponentBeginOverlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ADamageZone::OnComponentEndOverlap);

	Player = Cast<ABoomerShooterCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
}

void ADamageZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Damage zone once every N seconds
	if(PlayerInside && Player != nullptr)
	{
		CurrentDamageTime -= DeltaTime;

		if(CurrentDamageTime <= 0)
		{
			CurrentDamageTime = PlayerDamageTime;
			Player->TookDamage(PlayerDamage, GetActorLocation());
		}
	}
}

void ADamageZone::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(World != nullptr)
	{		
		if(Player != nullptr && OtherActor == Player)
		{
			PlayerInside = true;
		}
	}
}

void ADamageZone::OnComponentEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(World != nullptr)
	{
		if(Player != nullptr && OtherActor == Player)
		{
			PlayerInside = false;
		}
	}
}

