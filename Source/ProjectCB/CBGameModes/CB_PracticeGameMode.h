// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CB_PracticeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_PracticeGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ACB_PracticeGameMode();

protected:

	void BeginPlay() override;
	
};
