// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

class VERIFICATIONSERVER_API FTCPSocketListeningTh : public FRunnable
{
	FRunnableThread* Thread;

	static  FTCPSocketListeningTh* Runnable;

	static bool bThreadRun;

public:

	FTCPSocketListeningTh();
	~FTCPSocketListeningTh();

	virtual bool   Init();
	virtual uint32 Run();

	static FTCPSocketListeningTh* RunSocketListening();

	static void Shutdown();

};
