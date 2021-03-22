// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_GameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "ProjectCB/CBUI/CB_TextReaderComponent.h"

UCB_GameInstance::UCB_GameInstance() 
{
	UCB_TextReaderComponent* TextReader = CreateDefaultSubobject<UCB_TextReaderComponent>(TEXT("TextReaderComp"));

	ApiURL = TextReader->ReadFile("URLS/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();
}

void UCB_GameInstance::Shutdown()
{
	Super::Shutdown();

	if (AccessToken.Len() > 0)
	{
		TSharedRef<IHttpRequest> InvalidateTokensRequest = HttpModule->CreateRequest();
		InvalidateTokensRequest->SetURL(ApiURL + "/invalidatetokens");
		InvalidateTokensRequest->SetVerb("GET");
		InvalidateTokensRequest->SetHeader("Content Type", "application/json");
		InvalidateTokensRequest->SetHeader("Authorization", AccessToken);
		InvalidateTokensRequest->ProcessRequest();
	}
}

void UCB_GameInstance::SetCognitoTokens(FString NewAccessToken, FString NewIdToken, FString NewRefreshToken)
{
	AccessToken = NewAccessToken;
	IdToken = NewIdToken;
	RefreshToken = NewRefreshToken;

	//UE_LOG(LogTemp, Warning, TEXT("access token: %s"), *AccessToken);
	
	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UCB_GameInstance::RetrieveNewTokens, 1.0f, false, 3300.0f);
}

void UCB_GameInstance::RetrieveNewTokens()
{
	if (AccessToken.Len() > 0 && RefreshToken.Len() > 0)
	{
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);

		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
		{
			TSharedRef<IHttpRequest> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UCB_GameInstance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiURL + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
			RetrieveNewTokensRequest->SetHeader("Content Type", "application/json");
			RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
		}
		else 
		{
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UCB_GameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UCB_GameInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (!JsonObject->HasField("error"))
			{
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UCB_GameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UCB_GameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}
