// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "SocketObject.generated.h"

/**
 * Main network object, create socket, ñonnect-reconnect and so on
 */
UCLASS()
class SPIKY_CLIENT_API USocketObject : public UObject
{
	GENERATED_BODY()
	
	~USocketObject();

public:

	// tcp
	static FSocket* tcp_socket;
	// tcp server address
	static TSharedPtr<FInternetAddr> tcp_address;
	// connection state
	static bool bIsConnection;
	// reconnect if connection lost
	static void Reconnect();
	// check if server online
	static bool Alive();

	// udp
	static FSocket* udp_socket;
	// udp server address
	static TSharedPtr<FInternetAddr> udp_address;
	// we can not create a separate thread to listen UDP socket, unreal has FUdpSocketReceiver, create and delegate  
	static FUdpSocketReceiver* UDPReceiver;
	static void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	static void RunUdpSocketReceiver();
	static bool SendByUDP(google::protobuf::Message * message);

	static int32 tcp_local_port;
	static int32 udp_local_port;
	// Init sockets when ran the game, in GameInstance
	static void InitSocket(FString serverAddress, int32 tcp_local_p, int32 tcp_server_port, int32 udp_local_p, int32 udp_server_port);
	static bool ReadDelimitedFrom(google::protobuf::io::CodedInputStream* input, google::protobuf::MessageLite* message);
};