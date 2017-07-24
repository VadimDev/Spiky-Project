// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "InitialState.h"
#include "Protobufs/RegLogModels.pb.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "MainMenuWidgets.h"

void UInitialState::Handler(InitialState initialState)
{
	USpikyGameInstance::sessionId = initialState.sessionid();
	USpikyGameInstance::userLogin = initialState.login();

	GLog->Log(FString("Session ID: ") + USpikyGameInstance::sessionId.c_str());
	GLog->Log(FString("User login: ") + USpikyGameInstance::userLogin.c_str());

	USpikyGameInstance::DifferentMix->StopWaitingScreen();
	USpikyGameInstance::DifferentMix->wMainMenuWidgets->wPlayerName->SetText(FText::FromString(USpikyGameInstance::userLogin.c_str()));
	
	USpikyGameInstance::DifferentMix->ShowMainMenuScreen();
}