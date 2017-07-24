// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "WSWidgets.h"

#include "Runtime/Engine/Public/TimerManager.h"

void UWSWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	if (once)
	{
		once = false;
		GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, this, &UWSWidgets::GearsAnim, 0.01f, true);
	}

	wGear1 = Cast<UImage>(GetWidgetFromName(TEXT("Gear1")));
	wGear2 = Cast<UImage>(GetWidgetFromName(TEXT("Gear2")));
}

void UWSWidgets::GearsAnim()
{
	transform1.Angle += 1;
	wGear1->SetRenderTransform(transform1);

	transform2.Angle -= 1;
	wGear2->SetRenderTransform(transform2);
}

