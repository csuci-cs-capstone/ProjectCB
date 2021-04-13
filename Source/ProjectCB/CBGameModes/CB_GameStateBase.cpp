// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_GameStateBase.h"
#include "Net/UnrealNetwork.h"

void ACB_GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACB_GameStateBase, LatestEvent);

	DOREPLIFETIME(ACB_GameStateBase, WinningTeam);

	DOREPLIFETIME(ACB_GameStateBase, bTeamHasWon);

	DOREPLIFETIME(ACB_GameStateBase, BlueTeamSize);

	DOREPLIFETIME(ACB_GameStateBase, BlueTeamSizeAliveCount);

	DOREPLIFETIME(ACB_GameStateBase, YellowTeamSize);

	DOREPLIFETIME(ACB_GameStateBase, YellowTeamSizeAliveCount);
}

void ACB_GameStateBase::AssignPlayerToTeam(FString TeamName)
{
	if (TeamName == "blue")
	{
		BlueTeamSize += 1;
		BlueTeamSizeAliveCount = BlueTeamSize;
	}
	else if (TeamName == "yellow")
	{
		YellowTeamSize += 1;
		YellowTeamSizeAliveCount = YellowTeamSize;
	}
}

void ACB_GameStateBase::UpdateTeamSizeAliveCount(FString TeamName)
{
	if (TeamName == "blue")
	{
		BlueTeamSizeAliveCount--;
	}
	else if (TeamName == "yellow")
	{
		YellowTeamSizeAliveCount--;
	}
}
