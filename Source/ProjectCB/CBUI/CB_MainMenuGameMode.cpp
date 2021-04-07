// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_MainMenuGameMode.h"
#include "ProjectCB//CBUI/CB_MainMenuHUD.h"

ACB_MainMenuGameMode::ACB_MainMenuGameMode() 
{
	HUDClass = ACB_MainMenuHUD::StaticClass();
}