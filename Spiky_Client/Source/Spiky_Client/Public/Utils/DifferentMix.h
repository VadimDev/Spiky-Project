// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <string>
#include "DifferentMix.generated.h"

class UWidgetsContainer;
class UCanvasPanelSlot;
class URegWidgets;
class ULoginWidgets;
class USSButtonWidgets;
class USetServerWidgets;
class UUserWidget;
class UWSWidgets;
class UTexture2D;
class UMainMenuWidgets;
class UMainMenuChatWidgets;
class UCreateRoomWidgets;
class UGameRoomWidgets;
class UPingFpsWidgets;

/**
 * World singleton, stores references to widgets and rare functions
 */
UCLASS()
class SPIKY_CLIENT_API UDifferentMix : public UObject
{
	GENERATED_BODY()
	
	UDifferentMix(const FObjectInitializer& ObjectInitializer);

	UWidgetsContainer* tmpWidgetContainerRef;

	URegWidgets* tmpRegistrationRef;
	ULoginWidgets* tmpLoginScreenRef;
	USSButtonWidgets* tmpServerSettingsButtonRef;
	USetServerWidgets* tmpServerSettingsRef;
	UUserWidget* tmpTermsPrivacyRef;
	UWSWidgets*  tmpWaitingScreenRef;
	UMainMenuWidgets* tmpMainMenuRef;
	UMainMenuChatWidgets* tmpMainMenuChatRef;
	UCreateRoomWidgets* tmpCreateRoomRef;
	UGameRoomWidgets* tmpGameRoomRef;
	UPingFpsWidgets* tmpPingFpsRef;
	
public:

	virtual class UWorld* GetWorld() const override;

	void Init();

	UWidgetsContainer* wWidgetContainer;

	URegWidgets* wRegistration;
	ULoginWidgets* wLoginScreen;
	USSButtonWidgets* wServerSettingsButton;
	USetServerWidgets* wServerSettings;
	UUserWidget* wTermsPrivacy;
	UWSWidgets*  wWaitingScreen;
	UMainMenuWidgets* wMainMenuWidgets;
	UMainMenuChatWidgets* wMainMenuChatWidgets;
	UCreateRoomWidgets* wCreateRoomWidgets;
	UGameRoomWidgets* wGameRoomWidgets;
	UPingFpsWidgets* wPingFpsWidgets;

	UCanvasPanelSlot* registrationSlot;
	UCanvasPanelSlot* loginScreenSlot;
	UCanvasPanelSlot* serverSettingsButtonsSlot;
	UCanvasPanelSlot* serverSettingsSlot;
	UCanvasPanelSlot* TermsPrivacySlot;
	UCanvasPanelSlot* waitingScreenSlot;
	UCanvasPanelSlot* mainMenuSlot;
	UCanvasPanelSlot* mainMenuChatSlot;
	UCanvasPanelSlot* createRoomSlot;
	UCanvasPanelSlot* gameRoomSlot;
	UCanvasPanelSlot* pingFpsSlot;

	void HideAllWidgets();
	void ShowLoginScreen();
	void ShowMouse();
	void ShowRegistrationScreen();

	void StringCleaner(std::string & source, const std::string & availableSymbols);

	void RunWaitingScreen();
	void StopWaitingScreen();
	void ShowMainMenuScreen();
	void ShowGameRoom();

	UTexture2D* EmptyImage = nullptr;
	UTexture2D* GreenImage = nullptr;

	UTexture2D* CreateTexture(const std::string raw, bool alpha);

	int64 GetMS();
};