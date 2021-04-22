// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Delegates/Delegate.h"
#include "CB_GameStateBase.generated.h"

class ACB_PlayerUIHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHasTeamWon);
/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_GameStateBase : public AGameState
{
	GENERATED_BODY()
	
public:

	ACB_GameStateBase();

	UPROPERTY(Replicated)
		FString LatestEvent;

	UPROPERTY(Replicated)
		FString WinningTeam;
		
	UPROPERTY(Replicated)
		int CurrentGameplayMode;

	UPROPERTY(Replicated)
		bool bTeamHasWon = false;

	UPROPERTY(Replicated)
		int BlueTeamSize = 0;

	UPROPERTY(Replicated)
		int BlueTeamSizeAliveCount;

	UPROPERTY(Replicated)
		int YellowTeamSize = 0;

	UPROPERTY(Replicated)
		int YellowTeamSizeAliveCount;

	UPROPERTY(Replicated)
		int BlueTeamBallsCaptured = 0;

	UPROPERTY(Replicated)
		int YellowTeamBallsCaptured = 0;

	UPROPERTY()
		ACB_PlayerUIHUD* PlayerHUD;

	void AssignPlayerToTeam(FString TeamName);

	void UpdateTeamSizeAliveCount(FString TeamName);

	FString GetNextTeamToAssign();

	//How to call:
//YourDelegateName.Broadcast(YourParameters);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FHasTeamWon HasTeamWon;
};
