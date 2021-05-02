// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerUIWidget.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UCB_PlayerUIWidget::UCB_PlayerUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UCB_PlayerUIWidget::SetTeamAliveTextBlock(FString TextToSet)
{
	TeamAliveText->SetText(FText::FromString(TextToSet));
}

void UCB_PlayerUIWidget::SetEnemyAliveTextBlock(FString TextToSet)
{
	EnemyAliveText->SetText(FText::FromString(TextToSet));
}

void UCB_PlayerUIWidget::SetBallsCaptuedTextBlock(FString TextToSet)
{
	BallsCapturedText->SetText(FText::FromString(TextToSet));
}

void UCB_PlayerUIWidget::SetCountdownTextBlock(FString TextToSet)
{
	CountdownText->SetText(FText::FromString(TextToSet));
}

void UCB_PlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//TeamAliveTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("TeamAliveText"));
	//EnemyAliveTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("EnemyAliveText"));
	//BallsCapturedTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("BallsCapturedText"));

	//MatchmakingEventTextBlock->SetText(FText::FromString("Currently looking for a match"));
	//TeamAliveTextBlock->SetText(FText::FromString("0000000"));
	
}

void UCB_PlayerUIWidget::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCB_PlayerUIWidget, TeamAliveText);
	DOREPLIFETIME(UCB_PlayerUIWidget, EnemyAliveText);
	DOREPLIFETIME(UCB_PlayerUIWidget, BallsCapturedText);
	DOREPLIFETIME(UCB_PlayerUIWidget, CountdownText);
}

