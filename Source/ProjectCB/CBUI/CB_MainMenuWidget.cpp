// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_MainMenuWidget.h"
#include "ProjectCB/CBUI/CB_TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "ProjectCB/CB_GameInstance.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"



UCB_MainMenuWidget::UCB_MainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	UCB_TextReaderComponent* TextReader = CreateDefaultSubobject<UCB_TextReaderComponent>(TEXT("TextReaderComp"));
	LoginUrl = TextReader->ReadFile("../Content/URLS/LoginUrl.txt");
	ApiUrl = TextReader->ReadFile("../Content/URLS/ApiUrl.txt");
	CallbackUrl = TextReader->ReadFile("../Content/URLS/CallbackUrl.txt");
	RegionCode = TextReader->ReadFile("../Content/URLS/RegionCode.txt");

	HttpModule = &FHttpModule::Get();

	//Game Functionality
	SearchingForGame = false;
}

void UCB_MainMenuWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	bIsFocusable = true;

	WebBrowser = (UWebBrowser*) GetWidgetFromName(TEXT("WebLogin"));

	MainMenuPanel = (UPanelWidget*)GetWidgetFromName(TEXT("MainMenuPanel"));
	MainMenuPanel->SetVisibility(ESlateVisibility::Hidden);

	MatchmakingButton = (UButton*) GetWidgetFromName(TEXT("Play"));
	FScriptDelegate MatchmakingDelegate;
	MatchmakingDelegate.BindUFunction(this, "OnPlayButtonClicked");
	MatchmakingButton->OnClicked.Add(MatchmakingDelegate);

	MatchmakingEventTextBlock = (UTextBlock*)GetWidgetFromName(TEXT("GameStatusText"));

	FString AccessToken;
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr)
	{
		UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
		if (CBGameInstance != nullptr)
		{
			AccessToken = CBGameInstance->AccessToken;
		}
	}

	if (AccessToken.Len() > 0)
	{
		TSharedRef<IHttpRequest> GetPlayerDataRequest = HttpModule->CreateRequest();
		//TODO Look up BindUObject vs delegate cast
		GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnGetPlayerDataResponseReceived);
		GetPlayerDataRequest->SetURL(ApiUrl + "/getplayerdata");
		GetPlayerDataRequest->SetVerb("GET");
		GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
		GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
		GetPlayerDataRequest->ProcessRequest();
	}
	else 
	{
		IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();

		if (WebBrowserSingleton != nullptr)
		{
			TOptional<FString> DefaultContext;
			TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);

			if (CookieManager.IsValid())
			{
				CookieManager->DeleteCookies();
			}
		}

		//FString test = "www.gog.com";

		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("LOGGING %s"), *LoginUrl));

		WebBrowser->LoadURL(LoginUrl);

		FScriptDelegate LoginDelegate;
		LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
		WebBrowser->OnUrlChanged.Add(LoginDelegate);
	}

}

void UCB_MainMenuWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
	//GetWorld()->GetTimerManager().ClearTimer(SetAveragePlayerLatencyHandle);
	Super::NativeDestruct();
}

void UCB_MainMenuWidget::HandleLoginUrlChange()
{
	FString BrowserUrl = WebBrowser->GetUrl();
	FString Url;
	FString QueryParameters;

	if (BrowserUrl.Split("?", &Url, &QueryParameters))
	{
		if (Url.Equals(CallbackUrl))
		{
			FString ParameterName;
			FString ParameterValue;

			if (QueryParameters.Split("=", &ParameterName, &ParameterValue))
			{
				if (ParameterName.Equals("code"))
				{
					ParameterValue = ParameterValue.Replace(*FString("#"), *FString(""));

					TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
					RequestObj->SetStringField(ParameterName, ParameterValue);

					FString RequestBody;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

					if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
					{
						TSharedRef<IHttpRequest> ExchangeCodeForTokensRequest = HttpModule->CreateRequest();
						ExchangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnExchangeCodeForTokensResponseRecieved);
						ExchangeCodeForTokensRequest->SetURL(ApiUrl + "/exchangecodefortokens");
						ExchangeCodeForTokensRequest->SetVerb("POST");
						ExchangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
						ExchangeCodeForTokensRequest->SetContentAsString(RequestBody);
						ExchangeCodeForTokensRequest->ProcessRequest();
					}
				}
			}
		}
	}
}

void UCB_MainMenuWidget::OnPlayButtonClicked()
{
	MatchmakingButton->SetIsEnabled(false);

	FString AccessToken;
	FString MatchmakingTicketId;
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) 
	{
		UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
		if (CBGameInstance != nullptr)
		{
			AccessToken = CBGameInstance->AccessToken;
			MatchmakingTicketId = CBGameInstance->MatchmakingTicketId;
		}
	}

	if (SearchingForGame) 
	{
		GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
		SearchingForGame = false;

		if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0) 
		{
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", MatchmakingTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) 
			{
				TSharedRef<IHttpRequest> StopMatchmakingRequest = HttpModule->CreateRequest();
				StopMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnStopMatchmakingResponseReceived);
				StopMatchmakingRequest->SetURL(ApiUrl + "/stopmatchmaking");
				StopMatchmakingRequest->SetVerb("POST");
				StopMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StopMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StopMatchmakingRequest->SetContentAsString(RequestBody);
				StopMatchmakingRequest->ProcessRequest();
			}
			else 
			{
				UTextBlock* ButtonTextBlock = (UTextBlock*)MatchmakingButton->GetChildAt(0);
				ButtonTextBlock->SetText(FText::FromString("Play"));
				MatchmakingEventTextBlock->SetText(FText::FromString(""));

				MatchmakingButton->SetIsEnabled(true);
			}
		}
		else 
		{
			UTextBlock* ButtonTextBlock = (UTextBlock*)MatchmakingButton->GetChildAt(0);
			ButtonTextBlock->SetText(FText::FromString("Play"));
			MatchmakingEventTextBlock->SetText(FText::FromString(""));

			MatchmakingButton->SetIsEnabled(true);
		}
	}
	else 
	{
		if (AccessToken.Len() > 0) 
		{
			TSharedRef<FJsonObject> LatencyMapObj = MakeShareable(new FJsonObject);
			LatencyMapObj->SetNumberField(RegionCode, 60.0);

			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetObjectField("latencyMap", LatencyMapObj);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) 
			{
				TSharedRef<IHttpRequest> StartMatchmakingRequest = HttpModule->CreateRequest();
				StartMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnStartMatchmakingResponseReceived);
				StartMatchmakingRequest->SetURL(ApiUrl + "/startmatchmaking");
				StartMatchmakingRequest->SetVerb("POST");
				StartMatchmakingRequest->SetHeader("Content-Type", "application/json");
				StartMatchmakingRequest->SetHeader("Authorization", AccessToken);
				StartMatchmakingRequest->SetContentAsString(RequestBody);
				StartMatchmakingRequest->ProcessRequest();
			}
			else 
			{
				MatchmakingButton->SetIsEnabled(true);
			}
		}
		else 
		{
			MatchmakingButton->SetIsEnabled(true);
		}
	}
}

void UCB_MainMenuWidget::PollMatchmaking()
{
	FString AccessToken;
	FString MatchmakingTicketId;

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) 
	{
		UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
		if (CBGameInstance != nullptr)
		{
			AccessToken = CBGameInstance->AccessToken;
			MatchmakingTicketId = CBGameInstance->MatchmakingTicketId;
		}
	}

	if (AccessToken.Len() > 0 && MatchmakingTicketId.Len() > 0 && SearchingForGame) 
	{
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("ticketId", MatchmakingTicketId);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer)) 
		{
			TSharedRef<IHttpRequest> PollMatchmakingRequest = HttpModule->CreateRequest();
			PollMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnPollMatchmakingResponseReceived);
			PollMatchmakingRequest->SetURL(ApiUrl + "/pollmatchmaking");
			PollMatchmakingRequest->SetVerb("POST");
			PollMatchmakingRequest->SetHeader("Content-Type", "application/json");
			PollMatchmakingRequest->SetHeader("Authorization", AccessToken);
			PollMatchmakingRequest->SetContentAsString(RequestBody);
			PollMatchmakingRequest->ProcessRequest();
		}
	}
}

void UCB_MainMenuWidget::OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (JsonObject->HasField("access_token") && JsonObject->HasField("id_token") && JsonObject->HasField("refresh_token"))
			{
				UGameInstance* GameInstance = GetGameInstance();

				if (GameInstance != nullptr)
				{
					UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
					if (CBGameInstance != nullptr)
					{	
						FString AccessToken = JsonObject->GetStringField("access_token");
						FString IdToken = JsonObject->GetStringField("id_token");
						FString RefreshToken = JsonObject->GetStringField("refresh_token");

						CBGameInstance->SetCognitoTokens(AccessToken, IdToken, RefreshToken);

						WebBrowser->SetVisibility(ESlateVisibility::Hidden);
						MainMenuPanel->SetVisibility(ESlateVisibility::Visible);

						TSharedRef<IHttpRequest> GetPlayerDataRequest = HttpModule->CreateRequest();
						//TODO Look up BindUObject vs delegate cast
						GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UCB_MainMenuWidget::OnGetPlayerDataResponseReceived);
						GetPlayerDataRequest->SetURL(ApiUrl + "/getplayerdata");
						GetPlayerDataRequest->SetVerb("GET");
						GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
						GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
						GetPlayerDataRequest->ProcessRequest();
					}
				}
			}
		}
	}
}

void UCB_MainMenuWidget::OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) 
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		//If json object is able to made from the response
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) 
		{
			if (JsonObject->HasField("playerData")) 
			{
				TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("playerData");
				TSharedPtr<FJsonObject> WinsObject = PlayerData->GetObjectField("Wins");
				TSharedPtr<FJsonObject> LossesObject = PlayerData->GetObjectField("Losses");

				FString Wins = WinsObject->GetStringField("N");
				FString Losses = LossesObject->GetStringField("N");

				//WinsTextBlock->SetText(FText::FromString("Wins: " + Wins));
				//LossesTextBlock->SetText(FText::FromString("Losses: " + Losses));
			}
		}
	}
}

void UCB_MainMenuWidget::OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful) 
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject)) 
		{
			if (JsonObject->HasField("ticketId")) 
			{
				FString MatchmakingTicketId = JsonObject->GetStringField("ticketId");

				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
					if (CBGameInstance != nullptr)
					{
						CBGameInstance->MatchmakingTicketId = MatchmakingTicketId;

						GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UCB_MainMenuWidget::PollMatchmaking, 1.0f, true, 1.0f);
						SearchingForGame = true;

						UTextBlock* ButtonTextBlock = (UTextBlock*)MatchmakingButton->GetChildAt(0);
						ButtonTextBlock->SetText(FText::FromString("Cancel Search"));
						MatchmakingEventTextBlock->SetText(FText::FromString("Currently looking for a match"));
					}
				}
			}
		}
	}
	MatchmakingButton->SetIsEnabled(true);
}

void UCB_MainMenuWidget::OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) 
	{
		UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
		if (CBGameInstance != nullptr) 
		{
			CBGameInstance->MatchmakingTicketId = "";
		}
	}

	UTextBlock* ButtonTextBlock = (UTextBlock*)MatchmakingButton->GetChildAt(0);
	ButtonTextBlock->SetText(FText::FromString("Play"));
	MatchmakingEventTextBlock->SetText(FText::FromString(""));

	MatchmakingButton->SetIsEnabled(true);
}

void UCB_MainMenuWidget::OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && SearchingForGame) 
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) 
		{
			if (JsonObject->HasField("ticket")) 
			{
				//if (GEngine)
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player has ticket")));
				TSharedPtr<FJsonObject> Ticket = JsonObject->GetObjectField("ticket");
				FString TicketType = Ticket->GetObjectField("Type")->GetStringField("S");

				if (TicketType.Len() > 0) 
				{
					GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);
					SearchingForGame = false;

					UGameInstance* GameInstance = GetGameInstance();
					if (GameInstance != nullptr) 
					{
						UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
						if (CBGameInstance != nullptr)
						{
							CBGameInstance->MatchmakingTicketId = "";
						}
					}

					if (TicketType.Equals("MatchmakingSucceeded")) 
					{
						MatchmakingButton->SetIsEnabled(false);
						MatchmakingEventTextBlock->SetText(FText::FromString("Successfully found a match. Now connecting to the server..."));

						TSharedPtr<FJsonObject> GameSessionInfo = Ticket->GetObjectField("GameSessionInfo")->GetObjectField("M");
						FString IpAddress = GameSessionInfo->GetObjectField("IpAddress")->GetStringField("S");
						FString Port = GameSessionInfo->GetObjectField("Port")->GetStringField("N");

						TArray<TSharedPtr<FJsonValue>> Players = Ticket->GetObjectField("Players")->GetArrayField("L");
						TSharedPtr<FJsonObject> Player = Players[0]->AsObject()->GetObjectField("M");
						FString PlayerSessionId = Player->GetObjectField("PlayerSessionId")->GetStringField("S");
						FString PlayerId = Player->GetObjectField("PlayerId")->GetStringField("S");

						FString LevelName = IpAddress + ":" + Port;
						const FString& Options = "?PlayerSessionId=" + PlayerSessionId + "?PlayerId=" + PlayerId;
						//UE_LOG(LogTemp, Warning, TEXT("options: %s"), *Options);

						UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);
					}
					else 
					{
						UTextBlock* ButtonTextBlock = (UTextBlock*)MatchmakingButton->GetChildAt(0);
						ButtonTextBlock->SetText(FText::FromString("Play"));
						MatchmakingEventTextBlock->SetText(FText::FromString(TicketType + ". Please try again"));
					}
				}
			}
			else
			{
				//if (GEngine)
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player has is waiting on ticket")));
			}
		}
	}
}
