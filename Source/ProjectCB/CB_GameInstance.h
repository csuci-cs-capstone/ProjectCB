// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "CB_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API UCB_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCB_GameInstance();

	virtual void Shutdown() override;

	virtual void Init() override;

	UPROPERTY()
		FString AccessToken;

	UPROPERTY()
		FString IdToken;

	UPROPERTY()
		FString RefreshToken;

	UPROPERTY()
		FString MatchmakingTicketId;

	//Retrieve tokens for players every 55 minutes max
	UPROPERTY()
		FTimerHandle RetrieveNewTokensHandle;

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken);

	TDoubleLinkedList<float> PlayerLatencies;

	UPROPERTY()
		FTimerHandle GetResponseTimeHandle;

private:

	FHttpModule* HttpModule;

	UPROPERTY()
		FString ApiURL;

	UPROPERTY()
		FString RegionCode;

	UFUNCTION()
		void RetrieveNewTokens();

	UFUNCTION()
		void GetResponseTime();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetResponseTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
};
