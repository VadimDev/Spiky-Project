// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "SetServerWidgets.generated.h"

class UEditableTextBox;

UCLASS()
class SPIKY_CLIENT_API USetServerWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UEditableTextBox* wAddressBox = nullptr;
	UEditableTextBox* wPortBox = nullptr;
	
public:
	
	void SetAddress();
};
