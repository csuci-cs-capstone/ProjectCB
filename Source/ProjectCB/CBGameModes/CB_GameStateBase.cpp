// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_GameStateBase.h"
#include "ProjectCB/CBUI/CB_PlayerUIHUD.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "ProjectCB/CBPlayer/CB_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"


ACB_GameStateBase::ACB_GameStateBase() 
{
	
}

void ACB_GameStateBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	for (TActorIterator<APlayerController> It(World); It; ++It)
	{
		APlayerController* CurrentPlayerController = *It;

		if (CurrentPlayerController->IsLocalController())
		{
			this->m_localPlayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
			PlayerHUD = Cast<ACB_PlayerUIHUD>(this->m_localPlayerController->GetHUD());
		}
	}
}


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

	DOREPLIFETIME(ACB_GameStateBase, CurrentGameplayMode);

	DOREPLIFETIME(ACB_GameStateBase, BlueTeamBallsCaptured);

	DOREPLIFETIME(ACB_GameStateBase, YellowTeamBallsCaptured);
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

//For Testing Only - Flexmatch will handle this for us
FString ACB_GameStateBase::GetNextTeamToAssign()
{
	FString NextTeamName;

	if (BlueTeamSize > YellowTeamSize || BlueTeamSize == 5)
	{
		NextTeamName = "yellow";
	}

	if (BlueTeamSize < YellowTeamSize || YellowTeamSize == 5)
	{
		NextTeamName = "blue";
	}

	if (BlueTeamSize == YellowTeamSize)
	{
		NextTeamName = "blue";
	}
	return NextTeamName;
}

void ACB_GameStateBase::UpdateTeamGoalBox(FString TeamName, int CurrentAmount)
{
}

void ACB_GameStateBase::RefreshUIHUB_Implementation()
{

	if (PlayerHUD != nullptr)
	{
		//FString Test = "12";
		//PlayerHUD->SetTeamAlive(Test);
		ACB_PlayerState* CurrentPlayerState = Cast<ACB_PlayerState>(this->m_localPlayerController->PlayerState);
		if (CurrentPlayerState->Team == "blue")
		{
			FString AliveString = FString::FromInt(BlueTeamSizeAliveCount);
			PlayerHUD->SetTeamAlive(AliveString);
			FString EnemyString = FString::FromInt(YellowTeamSizeAliveCount);
			PlayerHUD->SetEnemyAlive(EnemyString);
		}
		else if (CurrentPlayerState->Team == "yellow")
		{
			FString AliveString = FString::FromInt(YellowTeamSizeAliveCount);
			PlayerHUD->SetTeamAlive(AliveString);
			FString EnemyString = FString::FromInt(BlueTeamSizeAliveCount);
			PlayerHUD->SetEnemyAlive(EnemyString);
		}
		FString NewCount = FString::FromInt(15);
		PlayerHUD->SetCountdownValue(NewCount);
	}
}


