// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "SpikyGameMode.h"
#include "SocketObject.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Protobufs/UtilityModels.pb.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"

void ASpikyGameMode::BeginPlay()
{
	Super::BeginPlay();

	GLog->Log("AClientGameMode::BeginPlay()");

	USpikyGameInstance* gameInstance = Cast<USpikyGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->DifferentMixInit(GetWorld());

	EnableInput(GetWorld()->GetFirstPlayerController());
	//InputComponent->BindAction("Q", IE_Pressed, this, &ASpikyGameMode::TestSendUPDMessage);

	USpikyGameInstance::DifferentMix->ShowLoginScreen();
	USpikyGameInstance::DifferentMix->ShowMouse();
}

void ASpikyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GLog->Log("AClientGameMode::EndPlay()");
}

void ASpikyGameMode::TestSendUPDMessage()
{
	GLog->Log("send ->>>");

	std::shared_ptr<Utility> utility(new Utility);
	utility->set_alive(true);

	USocketObject::SendByUDP(utility.get());
}