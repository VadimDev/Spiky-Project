// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "WidgetsContainer.generated.h"

class UCanvasPanel;

UCLASS()
class SPIKY_CLIENT_API UWidgetsContainer : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	UCanvasPanel * wCanvas = nullptr;
};
