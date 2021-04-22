// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerUIHUD.h"
#include "ProjectCB/CBUI/CB_PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

# define PLAYERUIBP_PATH "/Game/UIBP/Widgets/PlayerUI"

ACB_PlayerUIHUD::ACB_PlayerUIHUD() 
{
	this->bReplicates = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerUIObj(TEXT("/Game/UIBP/Widgets/PlayerUI"));

	if (PlayerUIObj.Succeeded())
	{
		PlayerUIWidgetClass = PlayerUIObj.Class;
	}
}

void ACB_PlayerUIHUD::SetTeamAlive(FString IncomingTextValue)
{
	if (CBPlayerUIWidget != nullptr)
	{
		CBPlayerUIWidget->SetTeamAliveTextBlock(IncomingTextValue);
	}
}

void ACB_PlayerUIHUD::SetEnemyAlive(FString IncomingTextValue)
{
	if (CBPlayerUIWidget != nullptr)
	{
		CBPlayerUIWidget->SetEnemyAliveTextBlock(IncomingTextValue);
	}
}

void ACB_PlayerUIHUD::SetBallsCaptured(FString IncomingTextValue)
{
	if (CBPlayerUIWidget != nullptr)
	{
		CBPlayerUIWidget->SetBallsCaptuedTextBlock(IncomingTextValue);
	}
}

void ACB_PlayerUIHUD::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerUIWidgetClass != nullptr)
	{
		CBPlayerUIWidget = CreateWidget<UCB_PlayerUIWidget>(GetWorld(), PlayerUIWidgetClass);

		if (CBPlayerUIWidget != nullptr)
		{
			CBPlayerUIWidget->AddToViewport();
		}
	}
}
