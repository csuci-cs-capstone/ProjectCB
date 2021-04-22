#include "CB_PlayerController.h"
#include "CB_PlayerCharacter.h"
#include "ProjectCB/CBUI/CB_PlayerUIHUD.h"
#include "ProjectCB/CBUI/CB_PlayerUIWidget.h"

ACB_PlayerController::ACB_PlayerController() 
{
	//this->bRespawnImmediately = false;
	this->SetReplicates(true);
}

void ACB_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	ACB_PlayerUIHUD* PlayerHUD = Cast<ACB_PlayerUIHUD>(GetHUD());

	/*if (PlayerHUD != nullptr)
	{
		FString Test = "12";
		PlayerHUD->SetTeamAlive(Test);
	}*/
}

void ACB_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
	
}

void ACB_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind Input Actions Here
	InputComponent->BindAxis("MoveVertical", this, &ACB_PlayerController::MoveVertical);
	InputComponent->BindAxis("MoveHorizontal", this, &ACB_PlayerController::MoveHorizontal);

	InputComponent->BindAxis("RotateCamera", this, &ACB_PlayerController::RotateCamera);

	InputComponent->BindAction("JumpAction", IE_Pressed, this, &ACB_PlayerController::JumpAction);
	InputComponent->BindAction("JumpAction", IE_Released, this, &ACB_PlayerController::StopJumpAction);

	InputComponent->BindAction("ShootAction", IE_Pressed, this, &ACB_PlayerController::ShootAction);
	InputComponent->BindAction("ShootAction", IE_Released, this, &ACB_PlayerController::StopShootAction);

	InputComponent->BindAction("AliveAction", IE_Pressed, this, &ACB_PlayerController::AliveAction);
}

void ACB_PlayerController::MoveVertical(float amount)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->MoveVertical(amount);
	}
}

void ACB_PlayerController::MoveHorizontal(float amount)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->MoveHorizontal(2 * amount);
	}
}

void ACB_PlayerController::RotateCamera(float amount)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->RotateCamera(amount);
	}
}

void ACB_PlayerController::JumpAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL) 
	{
		playerBody->JumpAction();
	}
}

void ACB_PlayerController::StopJumpAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->StopJumpAction();
	}
}

void ACB_PlayerController::ShootAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->ShootAction();
	}
}



void ACB_PlayerController::StopShootAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->StopShootAction();
	}
}



void ACB_PlayerController::AliveAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->AliveAction();
	}
}
