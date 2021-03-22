#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "GameFramework/GameMode.h"
#include "CB_Captureball_GameMode.generated.h"

//SERVER GAME MODE
//These structs will be passed through Amazon GamLift callbacks
//This struct for example will be passed to the game start callback function
//Bridges between server and GameLift services
USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FStartGameSessionState() 
	{
		Status = false;
	}
};

USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY();

	FUpdateGameSessionState() 
	{
	}
};

USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	long TerminationTime;

	FProcessTerminateState()
	{
		Status = false;
	}

};

USTRUCT()
struct FHealthCheckState 
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FHealthCheckState() 
	{
		Status = false;
	}
};

UCLASS()
class PROJECTCB_API ACB_Captureball_GameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ACB_Captureball_GameMode();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
		FStartGameSessionState StartGameSessionState;

	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
		FHealthCheckState HealthCheckState;
	
};
