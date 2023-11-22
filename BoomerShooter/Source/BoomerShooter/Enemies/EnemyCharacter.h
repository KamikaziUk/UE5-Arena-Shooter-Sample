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

#include "EnemyAIController.h"
#include "EnemyNavigationPoint.h"

#include "GameFramework/Character.h"

#include "EnemyCharacter.generated.h"

class ABoomerShooterCharacter;

UCLASS()
class BOOMERSHOOTER_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int StartingHealth;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	int PlayerScoreIncreaseOnKilled;

public:
	AEnemyCharacter();

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Navigation)
	AActor* LineMoverStartPoint;

	UPROPERTY(EditDefaultsOnly, Category = Navigation)
	AActor* LineMoverEndPoint;

	void DamageEnemy(int BulletDamage);
	bool IsDead();

protected:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

private:
	int Health;
	UWorld* World;
	ABoomerShooterCharacter* Player;
};
