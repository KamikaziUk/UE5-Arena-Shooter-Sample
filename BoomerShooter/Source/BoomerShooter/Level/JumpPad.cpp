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

#include "JumpPad.h"

#include "../Character/BoomerShooterCharacter.h"

#include "Kismet/GameplayStatics.h"

AJumpPad::AJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AJumpPad::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();

	Player = Cast<ABoomerShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	auto MeshComponent = FindComponentByClass(UStaticMeshComponent::StaticClass());
	if(IsValid(MeshComponent))
	{
		auto Mesh = Cast<UStaticMeshComponent>(MeshComponent);
		Mesh->OnComponentBeginOverlap.AddDynamic(this, &AJumpPad::OnComponentOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Mesh Component Found on Jumpad"));
	}
}

void AJumpPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJumpPad::OnComponentOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsValid(Player) && OtherActor == Player)
	{
		if(IsValid(JumpAudio))
		{
			UGameplayStatics::PlaySoundAtLocation(this, JumpAudio, GetActorLocation());
		}

		Player->LaunchCharacter(GetActorUpVector() * JumpForce, false, true);
	}
}