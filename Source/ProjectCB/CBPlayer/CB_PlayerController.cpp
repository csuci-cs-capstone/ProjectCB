// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerController.h"
#include "CB_PlayerCharacter.h"

ACB_PlayerController::ACB_PlayerController() 
{

}

void ACB_PlayerController::BeginPlay()
{
}

void ACB_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Bind Input Actions Here
	InputComponent->BindAxis("MoveVertical", this, &ACB_PlayerController::MoveVertical);
	InputComponent->BindAxis("MoveHorizontal", this, &ACB_PlayerController::MoveHorizontal);
	InputComponent->BindAxis("LookVertical", this, &ACB_PlayerController::LookVertical);
	InputComponent->BindAxis("LookHorizontal", this, &ACB_PlayerController::LookHorizontal);

	InputComponent->BindAxis("RotateCamera", this, &ACB_PlayerController::RotateCamera);

	InputComponent->BindAction("JumpAction", IE_Pressed, this, &ACB_PlayerController::JumpAction);
	InputComponent->BindAction("JumpAction", IE_Released, this, &ACB_PlayerController::StopJumpAction);

	InputComponent->BindAction("RunAction", IE_Pressed, this, &ACB_PlayerController::RunAction);
	InputComponent->BindAction("RunAction", IE_Released, this, &ACB_PlayerController::StopRunAction);

	InputComponent->BindAction("ShootAction", IE_Pressed, this, &ACB_PlayerController::ShootAction);
	InputComponent->BindAction("ShootAction", IE_Released, this, &ACB_PlayerController::StopShootAction);
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

void ACB_PlayerController::LookVertical(float amount)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->LookVertical(amount);
	}
}

void ACB_PlayerController::LookHorizontal(float amount)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->LookHorizontal(amount);
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

void ACB_PlayerController::RunAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->RunAction();
	}
}

void ACB_PlayerController::StopRunAction()
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->GetCharacter());

	if (playerBody != NULL)
	{
		playerBody->StopRunAction();
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
