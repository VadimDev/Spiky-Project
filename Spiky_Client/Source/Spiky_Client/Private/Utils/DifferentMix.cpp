// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "DifferentMix.h"
#include "SpikyGameInstance.h"
#include "WidgetsContainer.h"
#include "RegWidgets.h"
#include "LoginWidgets.h"
#include "SSButtonWidgets.h"
#include "SetServerWidgets.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include <algorithm>
#include "WSWidgets.h"
#include "Runtime/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "MainMenuWidgets.h"
#include "MainMenuChatWidgets.h"
#include "CreateRoomWidgets.h"
#include "GameRoomWidgets.h"
#include "PingFpsWidgets.h"

UDifferentMix::UDifferentMix(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UWidgetsContainer> WidgetContainer(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/WidgetContainer.WidgetContainer_C'"));

	if (WidgetContainer.Class != NULL)
	{
		tmpWidgetContainerRef = WidgetContainer.Class->GetDefaultObject<UWidgetsContainer>();
	}

	static ConstructorHelpers::FClassFinder<URegWidgets> RegistrationWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/Reg_W.Reg_W_C'"));

	if (RegistrationWidgets.Class != NULL)
	{
		tmpRegistrationRef = RegistrationWidgets.Class->GetDefaultObject<URegWidgets>();
	}

	static ConstructorHelpers::FClassFinder<ULoginWidgets> LoginWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/Login_W.Login_W_C'"));

	if (LoginWidgets.Class != NULL)
	{
		tmpLoginScreenRef = LoginWidgets.Class->GetDefaultObject<ULoginWidgets>();
	}

	static ConstructorHelpers::FClassFinder<USSButtonWidgets> SetServerButtonWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/SSButton_W.SSButton_W_C'"));

	if (SetServerButtonWidgets.Class != NULL)
	{
		tmpServerSettingsButtonRef = SetServerButtonWidgets.Class->GetDefaultObject<USSButtonWidgets>();
	}

	static ConstructorHelpers::FClassFinder<USetServerWidgets> ServerSettingsWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/SetServer_W.SetServer_W_C'"));

	if (ServerSettingsWidgets.Class != NULL)
	{
		tmpServerSettingsRef = ServerSettingsWidgets.Class->GetDefaultObject<USetServerWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> TermsPrivacyWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/Terms_Privacy_W.Terms_Privacy_W_C'"));

	if (TermsPrivacyWidgets.Class != NULL)
	{
		tmpTermsPrivacyRef = TermsPrivacyWidgets.Class->GetDefaultObject<UUserWidget>();
	}

	static ConstructorHelpers::FClassFinder<UWSWidgets> WaitingScreenWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/WS_W.WS_W_C'"));

	if (WaitingScreenWidgets.Class != NULL)
	{
		tmpWaitingScreenRef = WaitingScreenWidgets.Class->GetDefaultObject<UWSWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UMainMenuWidgets> MainMenuWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/MainMenu_W.MainMenu_W_C'"));

	if (MainMenuWidgets.Class != NULL)
	{
		tmpMainMenuRef = MainMenuWidgets.Class->GetDefaultObject<UMainMenuWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UMainMenuChatWidgets> chatMainMenuWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/ChatMain_W.ChatMain_W_C'"));

	if (chatMainMenuWidgets.Class != NULL)
	{
		tmpMainMenuChatRef = chatMainMenuWidgets.Class->GetDefaultObject<UMainMenuChatWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UCreateRoomWidgets> createRoomWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/CreateRoom_W.CreateRoom_W_C'"));

	if (createRoomWidgets.Class != NULL)
	{
		tmpCreateRoomRef = createRoomWidgets.Class->GetDefaultObject<UCreateRoomWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UGameRoomWidgets> gameRoomWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/GameRoom_W.GameRoom_W_C'"));

	if (gameRoomWidgets.Class != NULL)
	{
		tmpGameRoomRef = gameRoomWidgets.Class->GetDefaultObject<UGameRoomWidgets>();
	}

	static ConstructorHelpers::FClassFinder<UPingFpsWidgets> pingFpsWidgets(TEXT("WidgetBlueprint'/Game/Blueprints/Widgets/PingFPS_W.PingFPS_W_C'"));

	if (pingFpsWidgets.Class != NULL)
	{
		tmpPingFpsRef = pingFpsWidgets.Class->GetDefaultObject<UPingFpsWidgets>();
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> EmptyImageRef(TEXT("Texture2D'/Game/ProjectResources/Images/empty.empty'"));
	EmptyImage = EmptyImageRef.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> GreenImageRef(TEXT("Texture2D'/Game/ProjectResources/Images/G.G'"));
	GreenImage = GreenImageRef.Object;
}

class UWorld* UDifferentMix::GetWorld() const
{
	return USpikyGameInstance::world;
}

void UDifferentMix::Init()
{
	wWidgetContainer = CreateWidget<UWidgetsContainer>(GetWorld(), tmpWidgetContainerRef->GetClass());
	wWidgetContainer->AddToViewport();

	wRegistration = CreateWidget<URegWidgets>(GetWorld(), tmpRegistrationRef->GetClass());
	registrationSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wRegistration));
	registrationSlot->SetZOrder(0);
	registrationSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	registrationSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wRegistration->SetVisibility(ESlateVisibility::Hidden);

	wLoginScreen = CreateWidget<ULoginWidgets>(GetWorld(), tmpLoginScreenRef->GetClass());
	loginScreenSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wLoginScreen));
	loginScreenSlot->SetZOrder(0);
	loginScreenSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	loginScreenSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wLoginScreen->SetVisibility(ESlateVisibility::Hidden);

	wServerSettingsButton = CreateWidget<USSButtonWidgets>(GetWorld(), tmpServerSettingsButtonRef->GetClass());
	serverSettingsButtonsSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wServerSettingsButton));
	serverSettingsButtonsSlot->SetZOrder(3);
	serverSettingsButtonsSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	serverSettingsButtonsSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wServerSettingsButton->SetVisibility(ESlateVisibility::Hidden);

	wServerSettings = CreateWidget<USetServerWidgets>(GetWorld(), tmpServerSettingsRef->GetClass());
	serverSettingsSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wServerSettings));
	serverSettingsSlot->SetZOrder(1);
	serverSettingsSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	serverSettingsSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wServerSettings->SetVisibility(ESlateVisibility::Hidden);

	wTermsPrivacy = CreateWidget<UUserWidget>(GetWorld(), tmpTermsPrivacyRef->GetClass());
	TermsPrivacySlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wTermsPrivacy));
	TermsPrivacySlot->SetZOrder(1);
	TermsPrivacySlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	TermsPrivacySlot->SetOffsets(FMargin(0, 0, 0, 0));
	wTermsPrivacy->SetVisibility(ESlateVisibility::Hidden);

	wWaitingScreen = CreateWidget<UWSWidgets>(GetWorld(), tmpWaitingScreenRef->GetClass());
	waitingScreenSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wWaitingScreen));
	waitingScreenSlot->SetZOrder(1000); // max
	waitingScreenSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	waitingScreenSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wWaitingScreen->SetVisibility(ESlateVisibility::Hidden);

	wMainMenuWidgets = CreateWidget<UMainMenuWidgets>(GetWorld(), tmpMainMenuRef->GetClass());
	mainMenuSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wMainMenuWidgets));
	mainMenuSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	mainMenuSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wMainMenuWidgets->SetVisibility(ESlateVisibility::Hidden);

	wMainMenuChatWidgets = CreateWidget<UMainMenuChatWidgets>(GetWorld(), tmpMainMenuChatRef->GetClass());
	mainMenuChatSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wMainMenuChatWidgets));
	mainMenuChatSlot->SetZOrder(10);
	mainMenuChatSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	mainMenuChatSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wMainMenuChatWidgets->SetVisibility(ESlateVisibility::Hidden);

	wCreateRoomWidgets = CreateWidget<UCreateRoomWidgets>(GetWorld(), tmpCreateRoomRef->GetClass());
	createRoomSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wCreateRoomWidgets));
	createRoomSlot->SetZOrder(1);
	createRoomSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	createRoomSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wCreateRoomWidgets->SetVisibility(ESlateVisibility::Hidden);

	wGameRoomWidgets = CreateWidget<UGameRoomWidgets>(GetWorld(), tmpGameRoomRef->GetClass());
	gameRoomSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wGameRoomWidgets));
	gameRoomSlot->SetZOrder(1);
	gameRoomSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	gameRoomSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wGameRoomWidgets->SetVisibility(ESlateVisibility::Hidden);

	wPingFpsWidgets = CreateWidget<UPingFpsWidgets>(GetWorld(), tmpPingFpsRef->GetClass());
	pingFpsSlot = Cast<UCanvasPanelSlot>(wWidgetContainer->wCanvas->AddChild(wPingFpsWidgets));
	pingFpsSlot->SetZOrder(10);
	pingFpsSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
	pingFpsSlot->SetOffsets(FMargin(0, 0, 0, 0));
	wPingFpsWidgets->SetVisibility(ESlateVisibility::Hidden);
}

void UDifferentMix::HideAllWidgets()
{
	for (size_t i = 0; i < wWidgetContainer->wCanvas->GetChildrenCount(); i++)
	{
		wWidgetContainer->wCanvas->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UDifferentMix::ShowLoginScreen()
{
	HideAllWidgets();
	wLoginScreen->SetVisibility(ESlateVisibility::Visible);
	wServerSettingsButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDifferentMix::ShowMouse()
{
	// show mouse
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();
	MyController->bShowMouseCursor = true;
	MyController->bEnableClickEvents = true;
	MyController->bEnableMouseOverEvents = true;
}

void UDifferentMix::StringCleaner(std::string & source, const std::string & availableSymbols)
{
	source.erase(std::remove_if(source.begin(), source.end(),
		[&availableSymbols](const char ch)
	{
		if (availableSymbols.find(ch) != std::string::npos) return false;
		return true;
	}
	), source.end());
}

void UDifferentMix::RunWaitingScreen()
{
	wWaitingScreen->SetVisibility(ESlateVisibility::Visible);
}

void UDifferentMix::StopWaitingScreen()
{
	wWaitingScreen->SetVisibility(ESlateVisibility::Hidden);
}

void UDifferentMix::ShowRegistrationScreen()
{
	HideAllWidgets();
	wRegistration->SetVisibility(ESlateVisibility::Visible);
}

void UDifferentMix::ShowMainMenuScreen()
{
	HideAllWidgets();
	wMainMenuWidgets->SetVisibility(ESlateVisibility::Visible);
}

UTexture2D* UDifferentMix::CreateTexture(const std::string raw, bool alpha)
{
	int32 num = raw.length();
	const uint8_t * byte_array = reinterpret_cast<const uint8_t*>(raw.c_str());

	if (num != 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName(TEXT("ImageWrapper")));
		// Note: PNG format.  Other formats are supported
		IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(byte_array, num))
		{
			const TArray<uint8>* UncompressedBGRA = nullptr;
			TArray<uint8> UncompressedRGBA;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				// Create the UTexture for rendering
				UncompressedRGBA.AddZeroed(UncompressedBGRA->Num());
				for (int i = 0; UncompressedBGRA->Num() > i; i += 4) {
					UncompressedRGBA[i] = (*UncompressedBGRA)[i + 2];
					UncompressedRGBA[i + 1] = (*UncompressedBGRA)[i + 1];
					UncompressedRGBA[i + 2] = (*UncompressedBGRA)[i];
					UncompressedRGBA[i + 3] = (*UncompressedBGRA)[i + 3];
					if (alpha) {
						if ((UncompressedRGBA[i] + UncompressedRGBA[i + 1] + UncompressedRGBA[i + 2]) < 3) {
							UncompressedRGBA[i + 3] = 0;
						}
					}
				}

				UTexture2D* MyTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);

				// Fill in the source data from the file
				uint8* TextureData = (uint8*)MyTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				MyTexture->PlatformData->Mips[0].BulkData.Unlock();

				// Update the rendering resource from data.
				MyTexture->UpdateResource();
				return MyTexture;
			}
		}
	}
	return nullptr;
}

void UDifferentMix::ShowGameRoom()
{
	HideAllWidgets();
	// Close chat if open
	wMainMenuChatWidgets->SetVisibility(ESlateVisibility::Hidden);
	wMainMenuWidgets->bChatOpen = false;

	wGameRoomWidgets->SetVisibility(ESlateVisibility::Visible);
	wGameRoomWidgets->wToFirstTeamButton->SetVisibility(ESlateVisibility::Visible);
	wGameRoomWidgets->wToSecondTeamButton->SetVisibility(ESlateVisibility::Visible);
	wGameRoomWidgets->wToUndistributedTeamButton->SetVisibility(ESlateVisibility::Visible);
	wGameRoomWidgets->wStartButton->SetVisibility(ESlateVisibility::Visible);

	wGameRoomWidgets->AddPlayer(USpikyGameInstance::userLogin.c_str(), "undistributed");
}

int64 UDifferentMix::GetMS()
{
	FDateTime now = FDateTime::UtcNow();
	return now.ToUnixTimestamp() * 1000 + now.GetMillisecond();
}