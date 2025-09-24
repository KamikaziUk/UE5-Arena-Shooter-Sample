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

#include "EnemySpawnerManager.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

AEnemySpawnerManager::AEnemySpawnerManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawnerManager::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();

	SpawnWave();
}

void AEnemySpawnerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CurrentWave < SpawnWaves.Num())
	{
		bool IsWaveComplete = true;

		for(int i = 0; i < CurrentWaveCharacters.Num(); i++)
		{
			if(IsValid(CurrentWaveCharacters[i]))
			{
				if(!CurrentWaveCharacters[i]->IsDead())
				{
					IsWaveComplete = false;
					break;
				}
			}
		}

		if(IsWaveComplete)
		{
			CurrentWave += 1;
			SpawnWave();
		}
	}
}

void AEnemySpawnerManager::SpawnWave()
{
	if(IsValid(World))
	{
		if(CurrentWave < SpawnWaves.Num())
		{
			CurrentWaveCharacters.Empty();
			TArray<FSpawnCharacterData> WaveCharacters = SpawnWaves[CurrentWave].SpawnCharacters;

			for(int i = 0; i < WaveCharacters.Num(); i++)
			{
				if(IsValid(WaveCharacters[i].SpawnPoint))
				{
					AActor* spawnActor = WaveCharacters[i].SpawnPoint;

					const FRotator SpawnRotation = spawnActor->GetActorRotation();
					const FVector SpawnLocation = spawnActor->GetActorLocation() + FVector(0, 0, 100);

					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					auto* SpawnedCharacter = UAIBlueprintHelperLibrary::SpawnAIFromClass(World, WaveCharacters[i].SpawnCharacter, nullptr, SpawnLocation, SpawnRotation, true, nullptr);
					auto* EnemyCharacter = Cast<AEnemyCharacter>(SpawnedCharacter);
					EnemyCharacter->LineMoverStartPoint = WaveCharacters[i].LineMoverStartPoint;
					EnemyCharacter->LineMoverEndPoint = WaveCharacters[i].LineMoverEndPoint;

					CurrentWaveCharacters.Add(EnemyCharacter);
				}
			}
		}
		else
		{
			if(GoToDemoOnComplete)
			{
				UGameplayStatics::OpenLevel(World, LevelToComplete);
			}
		}
	}
}

void AEnemySpawnerManager::UpdateEnemies()
{
	for(int i = CurrentWaveCharacters.Num() - 1; i >= 0; i--)
	{
		if(!IsValid(CurrentWaveCharacters[i]) ||
			CurrentWaveCharacters[i]->IsDead())
		{
			CurrentWaveCharacters.RemoveAt(i);
		}
	}
}

TArray<AEnemyCharacter*> AEnemySpawnerManager::GetWaveCharacters()
{
	UpdateEnemies();

	if(CurrentWaveCharacters.Num() > 0)
	{
		return CurrentWaveCharacters;
	}
	
	return {};
}