// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_TextReaderComponent.h"
#include "Runtime/Core/Public//Misc//Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public//Misc/FileHelper.h"


// Sets default values for this component's properties
UCB_TextReaderComponent::UCB_TextReaderComponent()
{

}

FString UCB_TextReaderComponent::ReadFile(FString FilePath) 
{
	FString DirectoryPath = FPaths::ProjectConfigDir();
	FString FullPath = DirectoryPath + "/" + FilePath;
	FString Result;
	IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();

	if (File.FileExists(*FullPath)) 
	{
		FFileHelper::LoadFileToString(Result, *FullPath);
	}

	return Result;
}
