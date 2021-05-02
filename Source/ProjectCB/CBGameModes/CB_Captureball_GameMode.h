#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "GameFramework/GameMode.h"
#include "Runtime/Online/HTTP/Public/Http.h"
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

	UPROPERTY()
		FString MatchmakingConfigurationArn;

	TMap<FString, Aws::GameLift::Server::Model::Player> PlayerIdToPlayer;

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
		TerminationTime = 0.0L;
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

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;

public:

	UPROPERTY()
		FTimerHandle CountDownUntilGameOverHandle;

	UPROPERTY()
		FTimerHandle MatchStartCountDownHandle;

	UPROPERTY()
		FTimerHandle CaptureModeCountDownHandle;

	UPROPERTY()
		FTimerHandle EliminationCountDownHandle;

	UPROPERTY()
		FTimerHandle EndGameHandle;

	UPROPERTY()
		FTimerHandle PickAWinningTeamHandle;

	UPROPERTY()
		FTimerHandle HandleProcessTerminationHandle;

	UPROPERTY()
		FTimerHandle HandleGameSessionUpdateHandle;

	//For Testing Only
	int BlueTeamAmount = 0;

	int YellowTeamAmount = 0;

protected:

	void BeginPlay() override;

	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;


	//For Testing Only
	FString MockTeamAssign();

private:

	FHttpModule* HttpModule;

	UPROPERTY()
		FStartGameSessionState StartGameSessionState;

	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
		FHealthCheckState HealthCheckState;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString ServerPassword;

	UPROPERTY()
		int RemainingGameTime;

	UPROPERTY()
		bool GameSessionActivated;

	UFUNCTION()
		void BeginMatch();
	
	UFUNCTION()
		void StartCaptureMode();

	UFUNCTION()
		void EndCaptureMode();

	UFUNCTION()
		void StartEliminationMode();

	UFUNCTION()
		void EndEliminationMode();

	UFUNCTION()
		void CountDownUntilGameOver();

	UFUNCTION()
		void IsMatchOver();

	UFUNCTION()
		void EndGame();

	UFUNCTION()
		void PickAWinningTeam();
	
	UFUNCTION()
		void HandleProcessTermination();

	UFUNCTION()
		void HandleGameSessionUpdate();

	void OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
		
};
