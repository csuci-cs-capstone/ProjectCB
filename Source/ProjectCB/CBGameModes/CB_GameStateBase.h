// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CB_GameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Replicated)
		FString LatestEvent;

	UPROPERTY(Replicated)
		FString WinningTeam;
};
