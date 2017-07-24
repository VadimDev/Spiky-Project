// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "SpikyGameInstance.h"
#include "SocketObject.h"
#include "Config.h"
#include "ServerStatusCheckingTh.h"
#include "TCPSocketListeningTh.h"
#include "DifferentMix.h"

UWorld* USpikyGameInstance::world = nullptr;
UDifferentMix * USpikyGameInstance::DifferentMix = nullptr;

std::string USpikyGameInstance::secretKey = "";
std::string USpikyGameInstance::sessionId = "";
std::string USpikyGameInstance::userLogin = "";

void USpikyGameInstance::Init()
{
	GLog->Log("UClientGameInstance::Init()");

	USocketObject::InitSocket(Config::address.c_str(), Config::tcp_local_port, Config::tcp_server_port, Config::udp_local_port, Config::udp_server_port);

	// Start thread listening udp socket
	//USocketObject::RunUdpSocketReceiver();

	// Run a thread that checks the availability of the server
	FServerStatusCheckingTh::RunServerChecking();

	// Run thread listening tcp socket
	FTCPSocketListeningTh::RunSocketListening();
}

void USpikyGameInstance::Shutdown()
{
	GLog->Log("UClientGameInstance::Shutdown()");

	// Stop checking for server availability
	FServerStatusCheckingTh::Shutdown();

	// Stop thread listening tcp socket
	FTCPSocketListeningTh::Shutdown();
}

void USpikyGameInstance::DifferentMixInit(UWorld* the_world)
{
	GLog->Log("DifferentMixInit");

	world = the_world;

	DifferentMix = NewObject<UDifferentMix>(UDifferentMix::StaticClass());
	DifferentMix->AddToRoot();
	DifferentMix->Init();
}