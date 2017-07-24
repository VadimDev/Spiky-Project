#pragma once

#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

class SPIKY_CLIENT_API FServerStatusCheckingTh : public FRunnable
{
	// Singleton instance, can access the thread any time via static accessor, if it is active!
	static FServerStatusCheckingTh* Runnable;

	// Thread to run the worker FRunnable on
	FRunnableThread* Thread;

	// The way to stop
	static bool bThreadRun;

public:

	FServerStatusCheckingTh();
	~FServerStatusCheckingTh();

	// FRunnable interface
	virtual bool   Init();
	virtual uint32 Run();

	// Logics
	static FServerStatusCheckingTh* RunServerChecking();
	
	// Shuts down the thread. Static so it can easily be called from outside the thread context
	static void Shutdown();
};
