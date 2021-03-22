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



UCB_MainMenuWidget::UCB_MainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{
	UCB_TextReaderComponent* TextReader = CreateDefaultSubobject<UCB_TextReaderComponent>(TEXT("TextReaderComp"));
	LoginUrl = TextReader->ReadFile("../Content/URLS/LoginUrl.txt");
	ApiUrl = TextReader->ReadFile("../Content/URLS/ApiUrl.txt");
	CallbackUrl = TextReader->ReadFile("../Content/URLS/CallbackUrl.txt");

	HttpModule = &FHttpModule::Get();
}

void UCB_MainMenuWidget::NativeConstruct() 
{
	Super::NativeConstruct();

	bIsFocusable = true;

	WebBrowser = (UWebBrowser*) GetWidgetFromName(TEXT("Web_Login"));

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

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("LOGGING %s"), *LoginUrl));

	WebBrowser->LoadURL(LoginUrl);

	FScriptDelegate LoginDelegate;
	LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	WebBrowser->OnUrlChanged.Add(LoginDelegate);

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

void UCB_MainMenuWidget::OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if (!JsonObject->HasField("error"))
			{
				UGameInstance* GameInstance = GetGameInstance();

				if (GameInstance != nullptr)
				{
					UCB_GameInstance* CBGameInstance = Cast<UCB_GameInstance>(GameInstance);
					if (CBGameInstance != nullptr)
					{
						CBGameInstance->SetCognitoTokens(JsonObject->GetStringField("access_token"), JsonObject->GetStringField("id_token"), JsonObject->GetStringField("refresh_token"));
					}
				}
			}
		}
	}
}
