// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "SocketObject.h"
#include "Protobufs/MessageModels.pb.h"
#include "MessageEncoder.h"

FSocket* USocketObject::tcp_socket = nullptr;
TSharedPtr<FInternetAddr> USocketObject::tcp_address = nullptr;
bool USocketObject::bIsConnection = false;

FSocket* USocketObject::udp_socket = nullptr;
TSharedPtr<FInternetAddr> USocketObject::udp_address = nullptr;
FUdpSocketReceiver* USocketObject::UDPReceiver = nullptr;

int32 USocketObject::tcp_local_port = 0;
int32 USocketObject::udp_local_port = 0;

USocketObject::~USocketObject()
{
	GLog->Log("USocketObject::~USocketObject()");

	if (tcp_socket != nullptr || udp_socket != nullptr)
	{
		tcp_socket->Close();
		//UDPReceiver->Stop();

		delete tcp_socket;
		delete udp_socket;
	}
}

void USocketObject::InitSocket(FString serverAddress, int32 tcp_local_p, int32 tcp_server_port, int32 udp_local_p, int32 udp_server_port)
{
	int32 BufferSize = 2 * 1024 * 1024;

	tcp_local_port = tcp_local_p;
	udp_local_port = udp_local_p;

	/*
	tcp_socket = FTcpSocketBuilder("TCP_SOCKET")
		.AsNonBlocking() // Socket connect always success. Non blocking you say socket connect dont wait for response (Don?t block) so it will return true.
		.AsReusable()
		.WithReceiveBufferSize(BufferSize)
		.WithSendBufferSize(BufferSize)
		.Build();
	*/

	// tcp
	tcp_socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("TCP_SOCKET"), false);
	// create a proper FInternetAddr representation
	tcp_address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	// parse server address 
	FIPv4Address serverIP;
	FIPv4Address::Parse(serverAddress, serverIP);

	// and set 
	tcp_address->SetIp(serverIP.Value);
	tcp_address->SetPort(tcp_server_port);

	tcp_socket->Connect(*tcp_address);

	// set the initial connection state
	bIsConnection = Alive();

	// udp
	udp_address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	FIPv4Address::Parse(serverAddress, serverIP);

	udp_address->SetIp(serverIP.Value);
	udp_address->SetPort(udp_server_port);

	/*
	udp_socket = FUdpSocketBuilder("UDP_SOCKET")
		.AsReusable()
		.BoundToPort(udp_local_port)
		.WithBroadcast()
		.WithReceiveBufferSize(BufferSize)
		.WithSendBufferSize(BufferSize)
		.Build();
	*/
}

void USocketObject::RunUdpSocketReceiver()
{
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(udp_socket, ThreadWaitTime, TEXT("UDP_RECEIVER"));
	UDPReceiver->OnDataReceived().BindStatic(&USocketObject::Recv);
	UDPReceiver->Start();
}

void USocketObject::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	GLog->Log("Reveived UDP data");

	uint8_t * buffer = ArrayReaderPtr->GetData();
	size_t size = ArrayReaderPtr->Num();

	GLog->Log("Size of incoming data: " + FString::FromInt(size));

	google::protobuf::io::ArrayInputStream arr(buffer, size);
	google::protobuf::io::CodedInputStream input(&arr);

	std::shared_ptr<Wrapper> wrapper(new Wrapper);
	ReadDelimitedFrom(&input, wrapper.get());

	std::string msg;
	wrapper->SerializeToString(&msg);
	GLog->Log(msg.c_str());
}

bool USocketObject::SendByUDP(google::protobuf::Message * message)
{
	Wrapper wrapper;

	if (message->GetTypeName() == "Utility")
	{
		Utility * mes = static_cast<Utility*>(message);
		wrapper.set_allocated_utility(mes);
	}

	size_t size = wrapper.ByteSize() + 5; // include size, varint32 never takes more than 5 bytes
	uint8_t * buffer = new uint8_t[size];

	google::protobuf::io::ArrayOutputStream arr(buffer, size);
	google::protobuf::io::CodedOutputStream output(&arr);

	output.WriteVarint32(wrapper.ByteSize());
	wrapper.SerializeToCodedStream(&output);

	if (wrapper.has_utility())
	{
		wrapper.release_utility();
	}

	int32 bytesSent = 0;
	bool sentState = false;

	sentState = udp_socket->SendTo(buffer, output.ByteCount(), bytesSent, *udp_address);

	delete[] buffer;
	return sentState;
}

void USocketObject::Reconnect()
{
	tcp_socket->Close();

	uint32 OutIP;
	tcp_address->GetIp(OutIP);

	FString ip = FString::Printf(TEXT("%d.%d.%d.%d"), 0xff & (OutIP >> 24), 0xff & (OutIP >> 16), 0xff & (OutIP >> 8), 0xff & OutIP);

	InitSocket(ip, tcp_local_port, tcp_address->GetPort(), udp_local_port, udp_address->GetPort());
}

bool USocketObject::Alive()
{
	std::shared_ptr<Utility> utility(new Utility);
	utility->set_alive(true);

	return UMessageEncoder::Send(utility.get(), false, true);
}

bool USocketObject::ReadDelimitedFrom(google::protobuf::io::CodedInputStream * input, google::protobuf::MessageLite * message)
{
	// Read the size.
	uint32_t size;
	if (!input->ReadVarint32(&size)) return false;

	// Tell the stream not to read beyond that size.
	google::protobuf::io::CodedInputStream::Limit limit = input->PushLimit(size);

	// Parse the message.
	if (!message->MergeFromCodedStream(input)) return false;
	if (!input->ConsumedEntireMessage()) return false;

	// Release the limit.
	input->PopLimit(limit);

	return true;
}