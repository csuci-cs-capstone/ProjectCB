// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_GameStateBase.h"
#include "Net/UnrealNetwork.h"

void ACB_GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACB_GameStateBase, LatestEvent);

	DOREPLIFETIME(ACB_GameStateBase, WinningTeam);
}
