// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "CreateRoomWidgets.generated.h"

class UEditableTextBox;
class UButton;
class UComboBoxString;

UCLASS()
class SPIKY_CLIENT_API UCreateRoomWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	UEditableTextBox* wRoomNameTextBox = nullptr;
	UComboBoxString* wMapComboBox = nullptr;
	UComboBoxString* wGameTimeComboBox = nullptr;
	UComboBoxString* wPlayersNumberComboBox = nullptr;
	UButton* wCreateRoomButton = nullptr;

	UFUNCTION()
	void CreateRoomButtonClicked();

	UFUNCTION()
	void OnRoomNameTextChanged(const FText & text);

	bool bRoomNameOk = false;
};
