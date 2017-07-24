// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "PingFpsWidgets.generated.h"

class UTextBlock;

UCLASS()
class SPIKY_CLIENT_API UPingFpsWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	UTextBlock* wFps = nullptr;
	UTextBlock* wPing = nullptr;
};
