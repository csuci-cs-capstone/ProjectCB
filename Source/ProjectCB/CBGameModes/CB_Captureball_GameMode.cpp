#include "CB_Captureball_GameMode.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"
#include "ProjectCB/CBPlayer/CB_PlayerState.h"
#include "ProjectCB/CBGameModes/CB_GameStateBase.h"
#include "ProjectCB/CBUI/CB_TextReaderComponent.h"
#include "ProjectCB/CBUI/CB_PlayerUIHUD.h"
#include "ProjectCB/CBWorld/CB_GridGenerator.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/PlayerStartPIE.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"
#include "JsonUtilities.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), text));

ACB_Captureball_GameMode::ACB_Captureball_GameMode() 
{
	this->bDelayedStart = true;

	//Set the game mode controller and character to the intended player classes
	static ConstructorHelpers::FClassFinder<ACB_PlayerCharacter> PlayerCharacterBPClass(TEXT("/Game/PlayerBP/BP_CB_PlayerCharacter"));

	if (PlayerCharacterBPClass.Class != NULL) 
	{
		PlayerStateClass = ACB_PlayerState::StaticClass();
		GameStateClass = ACB_GameStateBase::StaticClass();
		PlayerControllerClass = ACB_PlayerController::StaticClass();
		DefaultPawnClass = PlayerCharacterBPClass.Class;
		HUDClass = ACB_PlayerUIHUD::StaticClass();
	}

	UCB_TextReaderComponent* TextReader = CreateAbstractDefaultSubobject<UCB_TextReaderComponent>(TEXT("TextReaderComp"));
	ApiUrl = TextReader->ReadFile("URLS/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();
	
	RemainingGameTime = 240;
	GameSessionActivated = false;

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

			FString MatchmakerData = GameSessionObj.GetMatchmakerData();

			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MatchmakerData);

			if (FJsonSerializer::Deserialize(Reader, JsonObject)) 
			{
				State->MatchmakingConfigurationArn = JsonObject->GetStringField("matchmakingConfigurationArn");

				TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
				for (TSharedPtr<FJsonValue> Team : Teams) 
				{
					TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
					FString TeamName = TeamObj->GetStringField("name");

					TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

					for (TSharedPtr<FJsonValue> Player : Players) 
					{
						TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
						FString PlayerId = PlayerObj->GetStringField("playerId");

						//TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
						//TSharedPtr<FJsonObject> Skill = Attributes->GetObjectField("skill");
						//FString SkillValue = Skill->GetStringField("valueAttribute");
						//auto SkillAttributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*SkillValue));

						Aws::GameLift::Server::Model::Player AwsPlayerObj;

						AwsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*PlayerId));
						AwsPlayerObj.SetTeam(TCHAR_TO_ANSI(*TeamName));
						//AwsPlayerObj.AddPlayerAttribute("skill", *SkillAttributeValue);

						State->PlayerIdToPlayer.Add(PlayerId, AwsPlayerObj);
					}
				}
			}
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

			State->Status = true;
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
				else if (Key.Equals("password")) 
				{
					ServerPassword = Value;
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
	GetWorldTimerManager().SetTimer(HandleGameSessionUpdateHandle, this, &ACB_Captureball_GameMode::HandleGameSessionUpdate, 1.0f, true, 5.0f);
	GetWorldTimerManager().SetTimer(HandleProcessTerminationHandle, this, &ACB_Captureball_GameMode::HandleProcessTermination, 1.0f, true, 5.0f);
	

	//GAMEMODE INIT WITHOUT AWS SDK

	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			CBGameState->HasTeamWon.AddDynamic(this, &ACB_Captureball_GameMode::IsMatchOver);
			CBGameState->CurrentGameplayMode = 0;
		}
	}
	//this->BeginMatch();
	//TODO set up countdown start timer here
	GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::BeginMatch, 3.0f, false, 3.0f);
}

void ACB_Captureball_GameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) 
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

#if WITH_GAMELIFT
	if (Options.Len() > 0) 
	{
		const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
		const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");

		if (PlayerSessionId.Len() > 0 && PlayerId.Len() > 0) 
		{
			Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
			DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

			auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
			if (DescribePlayerSessionsOutcome.IsSuccess()) 
			{
				auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
				int Count = 1;
				auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
				if (PlayerSessions != nullptr) 
				{
					auto PlayerSession = PlayerSessions[0];
					FString ExpectedPlayerId = PlayerSession.GetPlayerId();
					auto PlayerStatus = PlayerSession.GetStatus();

					if (ExpectedPlayerId.Equals(PlayerId) && PlayerStatus == Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED) 
					{
						auto AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));

						if (!AcceptPlayerSessionOutcome.IsSuccess()) 
						{
							ErrorMessage = "Unauthorized";
						}
					}
					else 
					{
						ErrorMessage = "Unauthorized";
					}
				}
				else 
				{
					ErrorMessage = "Unauthorized";
				}
			}
			else 
			{
				ErrorMessage = "Unauthorized";
			}
		}
		else 
		{
			ErrorMessage = "Unauthorized";
		}
	}
	else 
	{
		ErrorMessage = "Unauthorized";
	}
#endif
}

void ACB_Captureball_GameMode::Logout(AController* Exiting) 
{
#if WITH_GAMELIFT
	if (Exiting != nullptr) 
	{
		APlayerState* PlayerState = Exiting->PlayerState;
		if (PlayerState != nullptr) 
		{
			ACB_PlayerState* CBPlayerState = Cast<ACB_PlayerState>(PlayerState);
			const FString& PlayerSessionId = CBPlayerState->PlayerSessionId;
			if (PlayerSessionId.Len() > 0) 
			{
				Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
			}
		}
	}
#endif
	Super::Logout(Exiting);
}

FString ACB_Captureball_GameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString CurrentNewPlayerTeam;
#if WITH_GAMELIFT
#else
	//Only for testing, clients never will be using this in the real deal
	
	if (HasAuthority())
	{
		CurrentNewPlayerTeam = MockTeamAssign();
		ACB_PlayerState* CBPlayerState = Cast<ACB_PlayerState>(NewPlayerController->PlayerState);

		if (CBPlayerState != nullptr)
		{
			CBPlayerState->Team = CurrentNewPlayerTeam;
			ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(this->GameState);
			CBGameState->AssignPlayerToTeam(CurrentNewPlayerTeam);
		}
	}
#endif
	
	FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

#if WITH_GAMELIFT
	const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
	const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");

	if (NewPlayerController != nullptr) 
	{
		APlayerState* PlayerState = NewPlayerController->PlayerState;
		if (PlayerState != nullptr) 
		{
			ACB_PlayerState* CBPlayerState = Cast<ACB_PlayerState>(PlayerState);
			if (CBPlayerState != nullptr) 
			{
				CBPlayerState->PlayerSessionId = *PlayerSessionId;
				CBPlayerState->MatchmakingPlayerId = *PlayerId;

				if (StartGameSessionState.PlayerIdToPlayer.Num() > 0) 
				{
					if (StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) 
					{
						auto PlayerObj = StartGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->GetTeam();
						CurrentNewPlayerTeam = Team;
						CBPlayerState->Team = Team;
						ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(this->GameState);
						if (CBGameState != nullptr)
						{
							CBGameState->AssignPlayerToTeam(CurrentNewPlayerTeam);
						}
						
					}
				}
			}
		}
	}
#else
	//Post Finding Start and initializing
#endif

	return InitializedString;
}

AActor* ACB_Captureball_GameMode::ChoosePlayerStart_Implementation(AController* Player)
{

	// Choose a player start
	
	APlayerStart* FoundPlayerStart = nullptr;
	UClass* PawnClass = GetDefaultPawnClassForController(Player);
	APawn* PawnToFit = PawnClass ? PawnClass->GetDefaultObject<APawn>() : nullptr;
	
	ACB_PlayerCharacter* CBPlayerCharacter = Cast<ACB_PlayerCharacter>(PawnToFit);
	FString CurrentPlayerTeam;

	if (CBPlayerCharacter != nullptr)
	{
		FString::Printf(TEXT("FindingPlayerStart"));
		ACB_PlayerState* CBCurrentPlayerState = Cast<ACB_PlayerState>(Player->PlayerState);

		if (CBCurrentPlayerState != nullptr)
		{
			FString::Printf(TEXT("Current PlayerState Valid"));
			CurrentPlayerTeam = CBCurrentPlayerState->Team;
		}
	}
	
	TArray<APlayerStart*> UnOccupiedStartPoints;
	TArray<APlayerStart*> OccupiedStartPoints;
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* PlayerStart = *It;

			FString CurrentTag = PlayerStart->PlayerStartTag.ToString();

			if (CurrentTag == CurrentPlayerTeam)
			{
				FoundPlayerStart = PlayerStart;
				FoundPlayerStart->PlayerStartTag = "Taken";
				break;
			}
		}
	}
	
	if (FoundPlayerStart != nullptr)
	{
		FVector ActorLocation = FoundPlayerStart->GetActorLocation();
		FRotator ActorRotation = FoundPlayerStart->GetActorRotation();

		if (Player != nullptr)
		{
			Player->ClientSetRotation(ActorRotation, false);
			Player->ClientSetLocation(ActorLocation, ActorRotation);
		}
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("PlayerStart Not Found!"));
	}

	if (Player->GetPawn() != nullptr)
	{
		//Player->GetPawn()->SetActorRelativeRotation(ActorRotation);
		ACB_PlayerCharacter* CBPlayerCharacterIncoming = Cast<ACB_PlayerCharacter>(Player->GetPawn());
		if (CBPlayerCharacterIncoming != nullptr)
		{
			FVector ActorLocation = FoundPlayerStart->GetActorLocation();
			FRotator ActorRotation = FoundPlayerStart->GetActorRotation();
			CBPlayerCharacterIncoming->SetPlayerStartRotation();
			CBPlayerCharacterIncoming->SetActorLocation(ActorLocation);
			Player->Possess(PawnToFit);
			APlayerController* CurrentPlayerController = Cast<APlayerController>(Player);
			if (CurrentPlayerController != nullptr)
			{
				CurrentPlayerController->EnableInput(CurrentPlayerController);
				
			}
		}
	}

	return FoundPlayerStart;
}

AActor* ACB_Captureball_GameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	UWorld* World = GetWorld();
	AActor* BestStart = ChoosePlayerStart(Player);
	if (BestStart == nullptr)
	{
		// No player start found
		UE_LOG(LogGameMode, Log, TEXT("FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating"));

		// This is a bit odd, but there was a complex chunk of code that in the end always resulted in this, so we may as well just 
		// short cut it down to this.  Basically we are saying spawn at 0,0,0 if we didn't find a proper player start
		//BestStart = World->GetWorldSettings();
		
		while(BestStart != nullptr) 
		{
			BestStart = ChoosePlayerStart(Player);
		}
	}

	ACB_PlayerController* CBController = Cast<ACB_PlayerController>(Player);

	if (CBController != nullptr)
	{
		CBController->PlayerStartLocation = BestStart->GetActorLocation();
	}

	return BestStart;
}

FString ACB_Captureball_GameMode::MockTeamAssign()
{
	FString TeamToAssign = "blue";
	
	if (this->GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(this->GameState);

		if (CBGameState != nullptr)
		{
			TeamToAssign = CBGameState->GetNextTeamToAssign();
		}
	}
	
	return TeamToAssign;
}

//Load in and start match
void ACB_Captureball_GameMode::BeginMatch()
{
	this->StartMatch();

	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			UWorld* World = GetWorld();
			for (TActorIterator<APlayerController> It(World); It; ++It)
			{
				APlayerController* CurrentPlayerController = *It;

				if (CurrentPlayerController->IsLocalController())
				{
					CBGameState->m_localPlayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
					CBGameState->PlayerHUD = Cast<ACB_PlayerUIHUD>(CBGameState->m_localPlayerController->GetHUD());
					CBGameState->m_localPlayerController->SetPlayerControlEnabled(false);
					//CBGameState->m_localPlayerController->SetPlayerControlEnabled(true);
					break;
				}
			}

			CBGameState->CurrentGameplayMode = 1;
			CBGameState->RefreshUIHUB();
			GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::StartCaptureMode, 3.0f, false, 1.0f);
		}
	}
}

void ACB_Captureball_GameMode::StartCaptureMode()
{

	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			UWorld* World = GetWorld();
			for (TActorIterator<APlayerController> It(World); It; ++It)
			{
				APlayerController* CurrentPlayerController = *It;

				if (CurrentPlayerController != nullptr)
				{
					ACB_PlayerController* CBLocalPLayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
					CBLocalPLayerController->SetPlayerControlEnabled(true);
				}
			}

			//CBGameState->CurrentGameplayMode = 1;
			//CBGameState->RefreshUIHUB();
			CBGameState->EnableCount(true);
			CBGameState->UpdateCountDownTime(CountDownTime);
			//this->CountDownTime = 60;
			//this->EliminationTime = 60;
			GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::UpdateCaptureMode, 1.0f, true, 2.0f);
			
		}
	}

}

void ACB_Captureball_GameMode::UpdateCaptureMode()
{

	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			--CountDownTime;
			if (CountDownTime <= 0)
			{
				CBGameState->UpdateCountDownTime(0);
				GetWorldTimerManager().ClearTimer(MatchStartCountDownHandle);
				GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::EndCaptureMode, 1.0f, false, 1.0f);
			}
			else
			{
				CBGameState->UpdateCountDownTime(CountDownTime);
			}
		}
	}

}

void ACB_Captureball_GameMode::EndCaptureMode()
{
	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			UWorld* World = GetWorld();
			for (TActorIterator<APlayerController> It(World); It; ++It)
			{
				APlayerController* CurrentPlayerController = *It;

				if (CurrentPlayerController != nullptr)
				{
					ACB_PlayerController* CBLocalPLayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
					CBLocalPLayerController->SetPlayerControlEnabled(false);
				}
			}

			//CBGameState->CurrentGameplayMode = 1;
			//CBGameState->RefreshUIHUB();
			CBGameState->EnableCount(true);
			CBGameState->UpdateCountDownTime(EliminationTime);
			GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::StartEliminationMode, 3.0f, false, 5.0f);
		}
	}
}

void ACB_Captureball_GameMode::StartEliminationMode()
{
	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			UWorld* World = GetWorld();
			for (TActorIterator<APlayerController> It(World); It; ++It)
			{
				APlayerController* CurrentPlayerController = *It;

				if (CurrentPlayerController != nullptr)
				{
					ACB_PlayerController* CBLocalPLayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
					CBLocalPLayerController->SetPlayerControlEnabled(true);
					
					
				}
			}

			for (TActorIterator<ACB_GridGenerator> It(World); It; ++It)
			{
				ACB_GridGenerator* CBGenerator = *It;

				if (CBGenerator != nullptr)
				{
					CBGenerator->DisableBallGeneration();
					CBGenerator->EnableStageRemoval();
					break;
				}
			}

			CBGameState->CurrentGameplayMode = 2;
			CountDownTime = 60;
			//CBGameState->RefreshUIHUB();
			CBGameState->EnableCount(true);
			CBGameState->UpdateCountDownTime(CountDownTime);
			GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::UpdateEliminationMode, 1.0f, true, 2.0f);
		}
	}
}

void ACB_Captureball_GameMode::UpdateEliminationMode()
{
	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			--EliminationTime;
			if (EliminationTime <= 0)
			{
				CBGameState->UpdateCountDownTime(0);
				GetWorldTimerManager().ClearTimer(MatchStartCountDownHandle);
				GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::EndEliminationMode, 1.0f, false, 1.0f);
			}
			else
			{
				CBGameState->UpdateCountDownTime(EliminationTime);
			}
		}
	}
}

void ACB_Captureball_GameMode::EndEliminationMode()
{
	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			UWorld* World = GetWorld();
			for (TActorIterator<APlayerController> It(World); It; ++It)
			{
				APlayerController* CurrentPlayerController = *It;

				if (CurrentPlayerController != nullptr)
				{
					ACB_PlayerController* CBLocalPLayerController = Cast<ACB_PlayerController>(CurrentPlayerController);
					CBLocalPLayerController->SetPlayerControlEnabled(false);
				}
			}

			for (TActorIterator<ACB_GridGenerator> It(World); It; ++It)
			{
				ACB_GridGenerator* CBGenerator = *It;

				if (CBGenerator != nullptr)
				{
					CBGenerator->DisableStageRemoval();
					CBGenerator->DisableBallGeneration();
					break;
				}
			}

			//CBGameState->CurrentGameplayMode = 1;
			//CBGameState->RefreshUIHUB();
			CBGameState->EnableCount(true);
			CBGameState->UpdateCountDownTime(0);
			GetWorldTimerManager().SetTimer(MatchStartCountDownHandle, this, &ACB_Captureball_GameMode::PickAWinningTeam, 3.0f, false, 10.0f);
		}
	}
}

void ACB_Captureball_GameMode::CountDownUntilGameOver()
{
	if (GameState != nullptr) 
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			CBGameState->LatestEvent = FString::FromInt(this->RemainingGameTime) + "seconds until the game is over";
		}
	}

	if (this->RemainingGameTime > 0)
	{
		this->RemainingGameTime--;
	}
	else 
	{
		GetWorldTimerManager().ClearTimer(CountDownUntilGameOverHandle);
	}
}

void ACB_Captureball_GameMode::IsMatchOver()
{
	if (GameState != nullptr)
	{
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);

		if (CBGameState != nullptr)
		{
			if ((CBGameState->BlueTeamSizeAliveCount == 0 || CBGameState->YellowTeamSizeAliveCount == 0) && CBGameState->CurrentGameplayMode == 2)
			{
				CBGameState->bTeamHasWon = true;
				PickAWinningTeam();
			}
		}
	}
}

void ACB_Captureball_GameMode::EndGame()
{
	GetWorldTimerManager().ClearTimer(CountDownUntilGameOverHandle);
	GetWorldTimerManager().ClearTimer(EndGameHandle);
	GetWorldTimerManager().ClearTimer(PickAWinningTeamHandle);
	GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
	GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);
#if WITH_GAMELIFT
	Aws::GameLift::Server::TerminateGameSession();
	Aws::GameLift::Server::ProcessEnding();
	FGenericPlatformMisc::RequestExit(false);
#endif
}

//TODO Customize for our use case and conditions for winning
void ACB_Captureball_GameMode::PickAWinningTeam()
{
	GetWorldTimerManager().ClearTimer(CountDownUntilGameOverHandle);

#if WITH_GAMELIFT
	if (GameState != nullptr) {
		ACB_GameStateBase* CBGameState = Cast<ACB_GameStateBase>(GameState);
		if (CBGameState != nullptr)
		{
			CBGameState->LatestEvent = "GameEnded";

			if (CBGameState->BlueTeamSizeAliveCount == 0) 
			{
				CBGameState->WinningTeam = "Yellow";
			}
			else 
			{
				CBGameState->WinningTeam = "Blue";
			}

			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("winningTeam", CBGameState->WinningTeam);

			auto GetGameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
			if (GetGameSessionIdOutcome.IsSuccess()) 
			{
				RequestObj->SetStringField("gameSessionId", GetGameSessionIdOutcome.GetResult());

				FString RequestBody;
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
				if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) 
				{
					TSharedRef<IHttpRequest> RecordMatchResultRequest = HttpModule->CreateRequest();
					RecordMatchResultRequest->OnProcessRequestComplete().BindUObject(this, &ACB_Captureball_GameMode::OnRecordMatchResultResponseReceived);
					RecordMatchResultRequest->SetURL(ApiUrl + "/recordmatchresult");
					RecordMatchResultRequest->SetVerb("POST");
					RecordMatchResultRequest->SetHeader("Authorization", ServerPassword);
					RecordMatchResultRequest->SetHeader("Content-Type", "application/json");
					RecordMatchResultRequest->SetContentAsString(RequestBody);
					RecordMatchResultRequest->ProcessRequest();
				}
				else 
				{
					GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 5.0f);
				}
			}
			else 
			{
				GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 5.0f);
			}
		}
		else 
		{
			GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 5.0f);
		}
	}
	else 
	{
		GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 5.0f);
	}
#endif
}

void ACB_Captureball_GameMode::HandleProcessTermination()
{
	if (ProcessTerminateState.Status) 
	{
		GetWorldTimerManager().ClearTimer(CountDownUntilGameOverHandle);
		GetWorldTimerManager().ClearTimer(HandleProcessTerminationHandle);
		GetWorldTimerManager().ClearTimer(HandleGameSessionUpdateHandle);

		FString ProcessInterruptionMessage;

		if (ProcessTerminateState.TerminationTime <= 0L) 
		{
			ProcessInterruptionMessage = "Server process could shut down at any time";
		}
		else 
		{
			long TimeLeft = (long)(ProcessTerminateState.TerminationTime - FDateTime::Now().ToUnixTimestamp());
			ProcessInterruptionMessage = FString::Printf(TEXT("Server process scheduled to terminate in %ld seconds"), TimeLeft);
		}

		if (GameState != nullptr) 
		{
			ACB_GameStateBase* GameLiftTutorialGameState = Cast<ACB_GameStateBase>(GameState);
			if (GameLiftTutorialGameState != nullptr) 
			{
				GameLiftTutorialGameState->LatestEvent = ProcessInterruptionMessage;
			}
		}

		GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 10.0f);
	}
}

void ACB_Captureball_GameMode::HandleGameSessionUpdate()
{
	if (!GameSessionActivated)
	{
		if (StartGameSessionState.Status)
		{
			GameSessionActivated = true;

			//GetWorldTimerManager().SetTimer(PickAWinningTeamHandle, this, &ACB_Captureball_GameMode::PickAWinningTeam, 1.0f, false, (float)RemainingGameTime);
			//GetWorldTimerManager().SetTimer(CountDownUntilGameOverHandle, this, &ACB_Captureball_GameMode::CountDownUntilGameOver, 1.0f, true, 0.0f);
		}
	}
}

void ACB_Captureball_GameMode::OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	GetWorldTimerManager().SetTimer(EndGameHandle, this, &ACB_Captureball_GameMode::EndGame, 1.0f, false, 5.0f);
}
