#include "Spiky_Client.h"
#include "ServerStatusCheckingTh.h"
#include "SocketObject.h"

FServerStatusCheckingTh* FServerStatusCheckingTh::Runnable = nullptr;
bool FServerStatusCheckingTh::bThreadRun = false;

FServerStatusCheckingTh::FServerStatusCheckingTh()
{
	Thread = FRunnableThread::Create(this, TEXT("ServerStatusChecking"), 0, TPri_BelowNormal); 
}

FServerStatusCheckingTh::~FServerStatusCheckingTh()
{
	delete Thread;
	Thread = nullptr;
}

bool FServerStatusCheckingTh::Init()
{
	bThreadRun = true;
	return true;
}

uint32 FServerStatusCheckingTh::Run()
{
	while (bThreadRun)
	{
		FPlatformProcess::Sleep(1.f); // проверять каждую секунду

		if (!USocketObject::bIsConnection) // если нет соединения, переподключиться
		{
			USocketObject::Reconnect();
		}
		else
		{
			USocketObject::bIsConnection = USocketObject::Alive(); // если пакеты перестают доходить, значит сервер оффлайн
		}
		// отладка
		GLog->Log("Connect state (bIsConnection) = " + FString::FromInt((int32)USocketObject::bIsConnection) + " | FServerStatusCheckingTh::CheckServer");
	}
	return 0;
}

FServerStatusCheckingTh* FServerStatusCheckingTh::RunServerChecking()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FServerStatusCheckingTh();
	}
	return Runnable;
}

void FServerStatusCheckingTh::Shutdown()
{
	bThreadRun = false;

	GLog->Log("FServerStatusCheckingTh::Shutdown()");

	if (Runnable)
	{
		delete Runnable;
		Runnable = nullptr;
	}
}