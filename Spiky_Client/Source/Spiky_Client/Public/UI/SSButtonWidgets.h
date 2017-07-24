// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "SSButtonWidgets.generated.h"

class UButton;

UCLASS()
class SPIKY_CLIENT_API USSButtonWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UButton* wSettingsButton = nullptr;

	UFUNCTION()
	void SettingsButtonClicked();
};
