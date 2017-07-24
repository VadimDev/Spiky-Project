// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "MessageEncoder.h"
#include "Protobufs/GameModels.pb.h"
#include "SocketObject.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>

bool UMessageEncoder::Send(GameData* gameData)
{
	size_t size = gameData->ByteSize() + 5;
	uint8_t * buffer = new uint8_t[size];

	google::protobuf::io::ArrayOutputStream arr(buffer, size);
	google::protobuf::io::CodedOutputStream output(&arr);

	GLog->Log(FString::FromInt(gameData->ByteSize()));

	output.WriteVarint32(gameData->ByteSize());
	gameData->SerializeToCodedStream(&output);

	int32 bytesSent = 0;
	bool sentState = false;

	sentState = USocketObject::tcp_socket->Send(buffer, output.ByteCount(), bytesSent);

	delete[] buffer;
	return sentState;
}