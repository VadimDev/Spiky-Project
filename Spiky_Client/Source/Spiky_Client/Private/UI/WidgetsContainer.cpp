// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "WidgetsContainer.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "CanvasPanelSlot.h"

void UWidgetsContainer::NativeConstruct()
{
	Super::NativeConstruct();

	wCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("Canvas")));
}



