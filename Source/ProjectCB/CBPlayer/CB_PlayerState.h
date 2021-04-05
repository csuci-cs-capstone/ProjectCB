// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CB_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FString PlayerSessionId;
	
	UPROPERTY()
		FString MatchmakingPlayerId;

	UPROPERTY(Replicated)
		FString Team;
};
