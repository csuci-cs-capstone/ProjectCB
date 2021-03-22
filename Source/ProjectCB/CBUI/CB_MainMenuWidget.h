// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "CB_MainMenuWidget.generated.h"

class UWebBrowser;
/**
 * 
 */
UCLASS()
class PROJECTCB_API UCB_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UCB_MainMenuWidget(const FObjectInitializer& ObjectInitializer);
	
protected:

	virtual void NativeConstruct() override;

private:

	FHttpModule* HttpModule;

	UPROPERTY()
		FString LoginUrl;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString CallbackUrl;

	UPROPERTY()
		UWebBrowser* WebBrowser;

	UFUNCTION()
		void HandleLoginUrlChange();

	void OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
