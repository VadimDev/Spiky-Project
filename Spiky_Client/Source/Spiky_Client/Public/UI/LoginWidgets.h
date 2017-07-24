// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include <string>
#include "LoginWidgets.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;

UCLASS()
class SPIKY_CLIENT_API ULoginWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	bool bMailOk = false;
	bool bPassOk = false;

public:

	UButton* wSingUpButton = nullptr;
	UTextBlock* wInfoBlock = nullptr;

	UEditableTextBox* wMailTextBox = nullptr;
	UEditableTextBox* wPasswordTextBox = nullptr;

	UButton* wLoginButton = nullptr;
	UButton* wSettingsButton = nullptr;

	UFUNCTION()
	void SettingsButtonClicked();

	UFUNCTION()
	void SingUpButtonClicked();

	UFUNCTION()
	void LoginButtonClicked();

	UFUNCTION()
	void OnMailTextChanged(const FText & text);

	UFUNCTION()
	void OnPasswordTextChanged(const FText & text);

	FTimerHandle MessageTimerHandle;
	void HideErrorMessage();
	void ShowErrorMessage(FString msg);

	static std::string mail;
	static std::string password;
};
