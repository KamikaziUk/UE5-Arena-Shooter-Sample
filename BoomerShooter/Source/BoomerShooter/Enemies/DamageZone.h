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

#include "../Character/BoomerShooterCharacter.h"

#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "DamageZone.generated.h"

UCLASS()
class BOOMERSHOOTER_API ADamageZone : public AActor
{
	GENERATED_BODY()

	// Default components
	UPROPERTY(EditDefaultsOnly, Category = Collider)
	UBoxComponent* BoxCollider = nullptr;

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int PlayerDamage = 0;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float PlayerDamageTime = 0;

public:
	ADamageZone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION() 
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION() 
	void OnComponentEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	bool PlayerInside = false;
	float CurrentDamageTime = 0;
	ABoomerShooterCharacter* Player = nullptr;
	UWorld* World = nullptr;
};
