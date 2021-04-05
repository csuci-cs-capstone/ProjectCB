// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_MainMenuHUD.h"
#include "Blueprint/UserWidget.h"

#define MAINMENU_PATH "/Game/UIBP/Widgets/MainMenu"

ACB_MainMenuHUD::ACB_MainMenuHUD() 
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuObj(TEXT(MAINMENU_PATH));

	if (MainMenuObj.Succeeded())
	{
		MainMenuWidgetClass = MainMenuObj.Class;
	}
}

void ACB_MainMenuHUD::BeginPlay() 
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		PlayerController->bShowMouseCursor = true;
	}

	if (MainMenuWidgetClass != nullptr) 
	{
		UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);

		if (MainMenuWidget != nullptr) 
		{
			MainMenuWidget->AddToViewport();
			MainMenuWidget->SetFocus();
		}
	}
}
