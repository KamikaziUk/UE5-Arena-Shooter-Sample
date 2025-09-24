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

#include "EnemyAIController.h"

#include "EnemyCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	Player = UGameplayStatics::GetPlayerCharacter(World, 0);

	//auto PerceptionComp = GetPerceptionComponent();
	//PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPreceptionUpdated);
	//PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
}

void AEnemyAIController::OnPreceptionUpdated(const TArray<AActor*>& Actors)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "I see you!");
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* actor, FAIStimulus stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "I see you!");
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsValid(Player))
	{
		return;
	}

	auto PlayerPos = Player->GetActorLocation();
	auto ParentCharacter = Cast<AEnemyCharacter>(GetCharacter());

	if(EnemyType == EEnemyType::LineMover)
	{
		auto TargetLocation = FMath::ClosestPointOnSegment(PlayerPos,
			ParentCharacter->LineMoverStartPoint->GetActorLocation(),
			ParentCharacter->LineMoverEndPoint->GetActorLocation());

		MoveToLocation(TargetLocation);
	}
	else
	{
		float DistanceToPlayer = FVector::Distance(PlayerPos, ParentCharacter->GetActorLocation());
		if(DistanceToPlayer <= MinFollowDistance)
		{
			if(EnemyType == EEnemyType::Suicide ||
				EnemyType == EEnemyType::Chaser)
			{
				// Line trace to get a location close to the player, but not try to go inside the player
				FVector RayOrigin = PlayerPos;
				float RayLength = 255.0f;
				FVector RayEnd = RayOrigin + FVector(0, 0, -1) * RayLength;
				FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

				FHitResult Hit;
				World->LineTraceSingleByObjectType(
					OUT Hit,
					RayOrigin,
					RayEnd,
					FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
					TraceParams
				);

				AActor* ActorHit = Hit.GetActor();

				if(IsValid(ActorHit))
				{
					MoveToLocation(Hit.ImpactPoint);
				}
			}
		}
		else
		{
			MoveToLocation(ParentCharacter->GetActorLocation());
		}
	}
}