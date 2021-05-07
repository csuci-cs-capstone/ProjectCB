// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerState.h"
#include "ProjectCB/CBUI/CB_PlayerUIHUD.h"
#include "ProjectCB/CBUI/CB_PlayerUIWidget.h"
#include "Net/UnrealNetwork.h"

void ACB_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACB_PlayerState, Team);

	DOREPLIFETIME(ACB_PlayerState, bIsAlive);

	DOREPLIFETIME(ACB_PlayerState, HitScore);
}

