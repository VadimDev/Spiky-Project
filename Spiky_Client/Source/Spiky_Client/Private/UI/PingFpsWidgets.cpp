// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "PingFpsWidgets.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

void UPingFpsWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wFps = Cast<UTextBlock>(GetWidgetFromName(TEXT("fps")));
	wPing = Cast<UTextBlock>(GetWidgetFromName(TEXT("ping")));
}

