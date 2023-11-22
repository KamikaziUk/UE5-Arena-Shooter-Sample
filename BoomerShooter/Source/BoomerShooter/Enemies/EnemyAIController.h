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

#include "AIController.h"

#include "EnemyAIController.generated.h"

class AEnemyCharacter;

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Chaser        UMETA(DisplayName = "Chaser"), // Chases player then damages them
	Stationary    UMETA(DisplayName = "Stationary"), // Stands and shoots player
	Suicide       UMETA(DisplayName = "Suicide"), // Blows up on player
	LineMover     UMETA(DisplayName = "LineMover"), // Walks to closest point on line and shoots player
};

UCLASS()
class BOOMERSHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float MinFollowDistance;

	UPROPERTY(EditDefaultsOnly, Category = Enemy)
	EEnemyType EnemyType;
	
public:
	AEnemyAIController();

protected:
	UFUNCTION() void OnPreceptionUpdated(const TArray<AActor*>& Actors);
	UFUNCTION() void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UWorld* World;
	ACharacter* Player;
};
