// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CB_PlayerUIHUD.generated.h"


class UUserWidget;
class UCB_PlayerUIWidget;
/**
 * 
 */
UCLASS()
class PROJECTCB_API ACB_PlayerUIHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	ACB_PlayerUIHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UUserWidget> PlayerUIWidgetClass;

	UPROPERTY()
		UCB_PlayerUIWidget* CBPlayerUIWidget;

	void SetTeamAlive(FString IncomingTextValue);
	void SetEnemyAlive(FString IncomingTextValue);
	void SetBallsCaptured(FString IncomingTextValue);

protected:

	virtual void BeginPlay() override;

private:

	
};
