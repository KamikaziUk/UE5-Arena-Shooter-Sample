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

#include "../Character/BoomerShooterCharacter.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionPoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionPointActivatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionPointPlayerEnterDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionPointPlayerExitDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOOMERSHOOTER_API UInteractionPoint : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Interactions)
	float InteractDistance;

public:	
	UInteractionPoint();

	UPROPERTY(BlueprintAssignable, Category = Events)
	FInteractionPointActivatedDelegate OnInteractionPointActivated;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FInteractionPointPlayerEnterDelegate OnInteractionPointPlayerEnter;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FInteractionPointPlayerExitDelegate OnInteractionPointPlayerExit;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void OnPlayerInteractionPressed();

private:
	ABoomerShooterCharacter* Player;
	bool PlayerInRange;
};
