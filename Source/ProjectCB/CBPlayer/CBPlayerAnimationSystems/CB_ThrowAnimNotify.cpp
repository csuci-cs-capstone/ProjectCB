// Fill out your copyright notice in the Description page of Project Settings.

#include "CB_ThrowAnimNotify.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

void UCB_ThrowAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) 
{
	auto playerBody = Cast<ACB_PlayerCharacter>(MeshComp->GetOwner());
	playerBody->resetThrowing();
}