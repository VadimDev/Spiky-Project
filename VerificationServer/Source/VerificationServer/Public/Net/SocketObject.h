// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Networking.h"
#include "SocketSubsystem.h"
#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include "SocketObject.generated.h"

UCLASS()
class VERIFICATIONSERVER_API USocketObject : public UObject
{
	GENERATED_BODY()

	~USocketObject();

public:

	static FSocket* tcp_socket;
	static TSharedPtr<FInternetAddr> tcp_address;

	static void InitSocket(FString serverAddress, int32 tcp_server_port);
	static bool ReadDelimitedFrom(google::protobuf::io::CodedInputStream* input, google::protobuf::MessageLite* message);
};
