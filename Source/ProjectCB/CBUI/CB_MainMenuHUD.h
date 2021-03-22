// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CB_MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_MainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:

	ACB_MainMenuHUD();

protected:

	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
		TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
