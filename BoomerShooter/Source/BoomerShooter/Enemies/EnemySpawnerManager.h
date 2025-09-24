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

#include "EnemySpawnPoint.h"
#include "EnemyCharacter.h"

#include "GameFramework/Actor.h"

#include "EnemySpawnerManager.generated.h"

USTRUCT()
struct FSpawnCharacterData
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditAnywhere, Category = Spawn)
	AEnemySpawnPoint* SpawnPoint;

	UPROPERTY(EditAnywhere, Category = Spawn)
	AActor* LineMoverStartPoint;

	UPROPERTY(EditAnywhere, Category = Spawn)
	AActor* LineMoverEndPoint;

	UPROPERTY(EditAnywhere, Category = Spawn)
	TSubclassOf<AEnemyCharacter> SpawnCharacter;
};

USTRUCT()
struct FWaveData
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<FSpawnCharacterData> SpawnCharacters;
};

UCLASS()
class BOOMERSHOOTER_API AEnemySpawnerManager : public AActor
{
	GENERATED_BODY()

	// User properties
	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<FWaveData> SpawnWaves;

	UPROPERTY(EditDefaultsOnly, Category = Loading)
	FName LevelToComplete;

	UPROPERTY(EditAnywhere, Category = Loading)
	bool GoToDemoOnComplete;
	
public:	
	AEnemySpawnerManager();

	TArray<AEnemyCharacter*> GetWaveCharacters();
	void UpdateEnemies();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:
	void SpawnWave();

	int CurrentWave;
	TArray<AEnemyCharacter*> CurrentWaveCharacters;
	UWorld* World;
};
