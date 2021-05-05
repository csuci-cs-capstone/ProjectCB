// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PracticeGameMode.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"

ACB_PracticeGameMode::ACB_PracticeGameMode() 
{
	//Set the game mode controller and character to the intended player classes
	PlayerControllerClass = ACB_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<ACB_PlayerCharacter> PlayerCharacterBPClass(TEXT("/Game/PlayerBP/BP_CB_PlayerCharacterSingle"));

	if (PlayerCharacterBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerCharacterBPClass.Class;
	}

}

void ACB_PracticeGameMode::BeginPlay()
{
	
}
