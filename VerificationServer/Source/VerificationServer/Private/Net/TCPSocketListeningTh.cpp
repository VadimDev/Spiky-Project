// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "TCPSocketListeningTh.h"
#include "SocketObject.h"
#include "MessageDecoder.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/coded_stream.h>
#include "Protobufs/GameModels.pb.h"
#include "Async.h"

FTCPSocketListeningTh* FTCPSocketListeningTh::Runnable = nullptr;
bool FTCPSocketListeningTh::bThreadRun = false;

FTCPSocketListeningTh::FTCPSocketListeningTh()
{
	Thread = FRunnableThread::Create(this, TEXT("TCP_RECEIVER"), 0, TPri_BelowNormal); 
}

FTCPSocketListeningTh::~FTCPSocketListeningTh()
{
	delete Thread;
	Thread = nullptr;
}

bool FTCPSocketListeningTh::Init()
{
	bThreadRun = true;
	return true;
}

uint32 FTCPSocketListeningTh::Run()
{
	while (bThreadRun)
	{
		FPlatformProcess::Sleep(0.03f);

		if (!USocketObject::tcp_socket) return 0;

		//Binary Array!
		TArray<uint8> ReceivedData;

		uint32 Size;
		while (USocketObject::tcp_socket->HasPendingData(Size))
		{
			ReceivedData.Init(FMath::Min(Size, 65507u), Size);

			int32 Read = 0;
			USocketObject::tcp_socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		}

		if (ReceivedData.Num() > 0)
		{
			GLog->Log(FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()) + " | FTCPSocketListeningTh::Run");

			// преобразовать байты в protobuf
			uint8_t * buffer = ReceivedData.GetData();
			size_t size = ReceivedData.Num();

			google::protobuf::io::ArrayInputStream arr(buffer, size);
			google::protobuf::io::CodedInputStream input(&arr);

			bool protosize = true;
			while (protosize)
			{
				std::shared_ptr<GameData> gameData(new GameData);
				protosize = USocketObject::ReadDelimitedFrom(&input, gameData.get());

				GLog->Log(FString::FromInt(gameData->ByteSize()));

				AsyncTask(ENamedThreads::GameThread, [gameData]() {
					UMessageDecoder * Handler = NewObject<UMessageDecoder>(UMessageDecoder::StaticClass());
					Handler->SendProtoToDecoder(gameData.get());
				});
			}
		}
	}
	return 0;
}

FTCPSocketListeningTh* FTCPSocketListeningTh::RunSocketListening()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FTCPSocketListeningTh();
	}
	return Runnable;
}

void FTCPSocketListeningTh::Shutdown()
{
	bThreadRun = false;

	GLog->Log("FTCPSocketListeningTh::Shutdown()");

	if (Runnable)
	{
		delete Runnable;
		Runnable = nullptr;
	}
}