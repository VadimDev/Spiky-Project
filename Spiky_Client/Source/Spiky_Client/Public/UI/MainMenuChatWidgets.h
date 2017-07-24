// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MainMenuChatWidgets.generated.h"

class Chat;
class UButton;
class UScrollBox;
class UMultiLineEditableTextBox;
class URichText;

UCLASS()
class SPIKY_CLIENT_API UMainMenuChatWidgets : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	UButton* wEnterButton = nullptr;

	UFUNCTION()
	void EnterButtonClicked();

	UScrollBox * wChatScrollBox = nullptr;

	UMultiLineEditableTextBox* wChatTextBox = nullptr;

	void NewMessage(URichText* richText);

	void ScrollToEnd();
};