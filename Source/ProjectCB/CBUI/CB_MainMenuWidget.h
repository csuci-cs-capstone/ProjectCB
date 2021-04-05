// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "CB_MainMenuWidget.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;
class UPanelWidget;
/**
 * 
 */
UCLASS()
class PROJECTCB_API UCB_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UCB_MainMenuWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
		FTimerHandle PollMatchmakingHandle;
	
protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	FHttpModule* HttpModule;

	UPROPERTY()
		FString LoginUrl;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString CallbackUrl;

	UPROPERTY()
		FString RegionCode;

	UPROPERTY()
		UWebBrowser* WebBrowser;

	UPROPERTY()
		UPanelWidget* MainMenuPanel;

	UPROPERTY()
		UButton* MatchmakingButton;

	UPROPERTY()
		UTextBlock* WinsTextBlock;

	UPROPERTY()
		UTextBlock* LossesTextBlock;

	UPROPERTY()
		UTextBlock* PingTextBlock;

	UPROPERTY()
		bool SearchingForGame;

	UPROPERTY()
		UTextBlock* MatchmakingEventTextBlock;

	UFUNCTION()
		void HandleLoginUrlChange();

	UFUNCTION()
		void OnPlayButtonClicked();

	UFUNCTION()
		void PollMatchmaking();

	//Response handler functions
	void OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
