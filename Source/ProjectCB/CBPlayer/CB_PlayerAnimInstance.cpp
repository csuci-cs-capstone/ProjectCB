// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerAnimInstance.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

UCB_PlayerAnimInstance::UCB_PlayerAnimInstance() 
{
	//INIT ANIMS
}

/// @brief Sets this PlayerAnimIntstance properties according to the PlayerCharacter's properties, meant for blueprints.
void UCB_PlayerAnimInstance::UpdateAnimationProperties()
{
	ACB_PlayerCharacter* playerBody = Cast<ACB_PlayerCharacter>(TryGetPawnOwner());

	if (playerBody) 
	{
		this->bIsJumping = !playerBody->onGround();
		this->bIsDucking = playerBody->onDuck();

		this->bCurrentlyGrabbing = playerBody->onCatch();
		this->bIsThrowing = playerBody->onThrowing();

		this->moveSpeed = playerBody->groundMoveSpeed();

		if (moveSpeed == 0)
			this->bIsMoving = false;
		else
			this->bIsMoving = true;
	}
}

void UCB_PlayerAnimInstance::UpdateThrowAnimation()
{
	ACB_PlayerCharacter* playerBody = Cast<ACB_PlayerCharacter>(TryGetPawnOwner());
	playerBody->resetThrowing();
	this->bIsThrowing = false;
}

