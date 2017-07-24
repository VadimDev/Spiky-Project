// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "VerificationServerGameInstance.h"
#include "SocketObject.h"
#include "TCPSocketListeningTh.h"

void UVerificationServerGameInstance::Init()
{
	USocketObject::InitSocket("127.0.0.1", 7682);

	FTCPSocketListeningTh::RunSocketListening();
}

void UVerificationServerGameInstance::Shutdown()
{
	FTCPSocketListeningTh::Shutdown();
}