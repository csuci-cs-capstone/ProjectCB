#include "CB_Captureball_GameMode.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

ACB_Captureball_GameMode::ACB_Captureball_GameMode() 
{
	//Set the game mode controller and character to the intended player classes
	PlayerControllerClass = ACB_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<ACB_PlayerCharacter> PlayerCharacterBPClass(TEXT("/Game/PlayerBP/BP_CB_PlayerCharacter"));

	if (PlayerCharacterBPClass.Class != NULL) 
	{
		DefaultPawnClass = PlayerCharacterBPClass.Class;
	}

	//TODO set up AWS gameliftSDK and create a struct to bridge the sever functionality here.
	
}