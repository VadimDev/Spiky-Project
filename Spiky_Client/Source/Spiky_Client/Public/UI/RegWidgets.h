// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "RegWidgets.generated.h"

class UButton;
class UImage;
class UEditableTextBox;
class UTextBlock;
class UTexture2D;

UCLASS()
class SPIKY_CLIENT_API URegWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	URegWidgets(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

public:

	UButton* wReloadCaptchaButton = nullptr;
	UImage* wCaptchaImage = nullptr;

	UImage* wLoginImage = nullptr;
	UImage* wPassImage = nullptr;
	UImage* wMailImage = nullptr;
	UImage* wCaptchaCheckImage = nullptr;

	UTexture2D* accept_tex = nullptr;
	UTexture2D* denied_tex = nullptr;
	UTexture2D* empty_tex = nullptr;

	UEditableTextBox* wLoginTextBox = nullptr;
	UEditableTextBox* wPasswordTextBox = nullptr;
	UEditableTextBox* wMainTextBox = nullptr;
	UEditableTextBox* wCaptchaTextBox = nullptr;

	UTextBlock* wInfoBlock = nullptr;

	UButton* wShowTermsPrivacyButton = nullptr;

	UButton* wCloseButton = nullptr;

	UButton* wSingUpButton = nullptr;

	UFUNCTION()
	void SingUpButtonClicked();

	UFUNCTION()
	void CloseButtonClicked();

	UFUNCTION()
	void ShowTermPrivacyClicked();

	UFUNCTION()
	void ReloadCaptchaClicked();

	UFUNCTION()
	void OnLoginTextChanged(const FText & text);

	UFUNCTION()
	void OnPasswordTextChanged(const FText & text);

	UFUNCTION()
	void OnMailTextChanged(const FText & text);

	UFUNCTION()
	void OnCaptchaTextChanged(const FText & text);

	bool bLoginOk = false;
	bool bPassOk = false;
	bool bMailOk = false;
	bool bCaptchaOk = false;
};
