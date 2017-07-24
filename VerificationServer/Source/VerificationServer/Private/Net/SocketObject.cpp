// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "SocketObject.h"
#include "MessageEncoder.h"

FSocket* USocketObject::tcp_socket = nullptr;
TSharedPtr<FInternetAddr> USocketObject::tcp_address = nullptr;

USocketObject::~USocketObject()
{
	if (tcp_socket != nullptr)
	{
		tcp_socket->Close();
		delete tcp_socket;
	}
}

void USocketObject::InitSocket(FString serverAddress, int32 tcp_server_port)
{
	int32 BufferSize = 2 * 1024 * 1024;

	tcp_socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP_SOCKET"), false);
	tcp_address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	FIPv4Address serverIP;
	FIPv4Address::Parse(serverAddress, serverIP);

	tcp_address->SetIp(serverIP.Value);
	tcp_address->SetPort(tcp_server_port);

	tcp_socket->Connect(*tcp_address);
}

bool USocketObject::ReadDelimitedFrom(google::protobuf::io::CodedInputStream* input, google::protobuf::MessageLite* message)
{
	uint32_t size;
	if (!input->ReadVarint32(&size)) return false;

	google::protobuf::io::CodedInputStream::Limit limit = input->PushLimit(size);

	if (!message->MergeFromCodedStream(input)) return false;
	if (!input->ConsumedEntireMessage()) return false;
	
	input->PopLimit(limit);

	return true;
}
