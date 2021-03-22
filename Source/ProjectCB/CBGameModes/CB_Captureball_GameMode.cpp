#include "CB_Captureball_GameMode.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

ACB_Captureball_GameMode::ACB_Captureball_GameMode() 
{
	//Set the game mode controller and character to the intended player classes
	PlayerControllerClass = ACB_PlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<ACB_PlayerCharacter> PlayerCharacterBPClass(TEXT("/Game/PlayerBP/BP_CB_PlayerCharacter"));

	if (PlayerCharacterBPClass.Class != NULL) 
	{
		DefaultPawnClass = PlayerCharacterBPClass.Class;
	}
	
}

void ACB_Captureball_GameMode::BeginPlay()
{
	Super::BeginPlay();

	//Run Code in server configuration only.
#if WITH_GAMELIFT
	auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

	if (InitSDKOutcome.IsSuccess())
	{
		//Invoked when server wants to start a match, this is one of the GameLift callbacks
		//GameSessionObj is passed by the GameLift Service will have info on players
		//Params will contain a State struct(in header) 
		auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
		{
			FStartGameSessionState* State = (FStartGameSessionState*)Params;

			State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
		};

		//Can update the game and add more incoming players
		auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params) 
		{
			FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;
		};

		//Invoke on Unhealthy server process, a spot interruption of EC2 instance, Fleet Capacity changes
		auto OnProcessTerminate = [](void* Params)
		{
			FProcessTerminateState* State = (FProcessTerminateState*)Params;
			
			auto GetTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();

			if (GetTerminationTimeOutcome.IsSuccess()) 
			{
				State->TerminationTime = GetTerminationTimeOutcome.GetResult();
			}

			auto ProcessEndingOutcome = Aws::GameLift::Server::ProcessEnding();

			//Basic Cleanup, May want to look into a better clean up if/when needed
			//GameLift server process termination
			if (ProcessEndingOutcome.IsSuccess())
			{
				State->Status = true;
				FGenericPlatformMisc::RequestExit(false);
			}
		};

		//Gets Invoked every 60(s) to check if the server is healthy
		//returning true means healthy else the server is not and OnProcessTerminate is called
		auto OnHealthCheck = [](void* Params)
		{
			FHealthCheckState* State = (FHealthCheckState*)Params;
			State->Status = true;

			return State->Status;
		};

		//SERVER COMMAND LINE
		//Handle telling which port our sever will run on the instance 
		TArray<FString> CommandLineTokens;
		TArray<FString> CommandLineSwitches;
		int Port = FURL::UrlConfig.DefaultPort;

		// Example Run: CaptureBallServer.exe token -port7777
		FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

		for (FString Str : CommandLineSwitches)
		{
			FString Key;
			FString Value;

			if (Str.Split("=", &Key, &Value))
			{
				if (Key.Equals("port"))
				{
					Port = FCString::Atoi(*Value);
				}
			}
		}

		//Files that Aws will be able to store for future access
		//Everytime a gamesession starts GameLift generates files for logging.
		const char* LogFile = "aLogFile.txt";
		const char** LogFiles = &LogFile;
		auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

		auto Params = new Aws::GameLift::Server::ProcessParameters(
			OnStartGameSession,
			&StartGameSessionState,
			OnUpdateGameSession,
			&UpdateGameSessionState,
			OnProcessTerminate,
			&ProcessTerminateState,
			OnHealthCheck,
			&HealthCheckState,
			Port,
			*LogParams
		);

		//Can call IsSuccess and grab the error on it if needed in future
		auto ProcessReadyOutcome = Aws::GameLift::Server::ProcessReady(*Params);
	}
#endif
}
