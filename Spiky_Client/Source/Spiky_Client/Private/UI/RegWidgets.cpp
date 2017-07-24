// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "RegWidgets.h"
#include "Protobufs/RegLogModels.pb.h"
#include "MessageEncoder.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

URegWidgets::URegWidgets(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> accept_ref(TEXT("Texture2D'/Game/ProjectResources/Images/accept.accept'"));

	accept_tex = accept_ref.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> denied_ref(TEXT("Texture2D'/Game/ProjectResources/Images/denied.denied'"));

	denied_tex = denied_ref.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> empty_ref(TEXT("Texture2D'/Game/ProjectResources/Images/empty.empty'"));

	empty_tex = empty_ref.Object;
}

void URegWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wReloadCaptchaButton = Cast<UButton>(GetWidgetFromName(TEXT("ReloadCaptchaButton")));
	wReloadCaptchaButton->OnClicked.AddDynamic(this, &URegWidgets::ReloadCaptchaClicked);

	wCaptchaImage = Cast<UImage>(GetWidgetFromName(TEXT("CaptchaImage")));

	wLoginTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("LoginTextBox")));
	wLoginTextBox->OnTextChanged.AddDynamic(this, &URegWidgets::OnLoginTextChanged);

	wPasswordTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("PasswordTextBox")));
	wPasswordTextBox->OnTextChanged.AddDynamic(this, &URegWidgets::OnPasswordTextChanged);

	wMainTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("MailTextBox")));
	wMainTextBox->OnTextChanged.AddDynamic(this, &URegWidgets::OnMailTextChanged);

	wCaptchaTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("CaptchaTextBox")));
	wCaptchaTextBox->OnTextChanged.AddDynamic(this, &URegWidgets::OnCaptchaTextChanged);

	wLoginImage = Cast<UImage>(GetWidgetFromName(TEXT("LoginImage")));
	wPassImage = Cast<UImage>(GetWidgetFromName(TEXT("PasswordImage")));
	wMailImage = Cast<UImage>(GetWidgetFromName(TEXT("MailImage")));
	wCaptchaCheckImage = Cast<UImage>(GetWidgetFromName(TEXT("CaptchaCheckImage")));
	wInfoBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBlock")));

	wShowTermsPrivacyButton = Cast<UButton>(GetWidgetFromName(TEXT("TermsPrivacy")));
	wShowTermsPrivacyButton->OnClicked.AddDynamic(this, &URegWidgets::ShowTermPrivacyClicked);

	wCloseButton = Cast<UButton>(GetWidgetFromName(TEXT("CloseButton")));
	wCloseButton->OnClicked.AddDynamic(this, &URegWidgets::CloseButtonClicked);

	wSingUpButton = Cast<UButton>(GetWidgetFromName(TEXT("SingUpButton")));
	wSingUpButton->OnClicked.AddDynamic(this, &URegWidgets::SingUpButtonClicked);
}

void URegWidgets::CloseButtonClicked()
{
	USpikyGameInstance::DifferentMix->ShowLoginScreen();
}

void URegWidgets::ShowTermPrivacyClicked()
{
	USpikyGameInstance::DifferentMix->wTermsPrivacy->SetVisibility(ESlateVisibility::Visible);
}

void URegWidgets::ReloadCaptchaClicked()
{
	std::shared_ptr<InputChecking> inputChecking(new InputChecking);
	inputChecking->set_getcaptcha(true);

	UMessageEncoder::Send(inputChecking.get(), false, true);

	wCaptchaTextBox->SetText(FText::FromString(""));
	bCaptchaOk = false;
}

void URegWidgets::OnLoginTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");

	wLoginTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 3)
	{
		wLoginImage->SetBrushFromTexture(denied_tex);
		wInfoBlock->SetText(FText::FromString("Error : Too short login"));
		return;
	}

	wInfoBlock->SetText(FText::FromString(" "));

	wLoginImage->SetBrushFromTexture(empty_tex);

	std::shared_ptr<InputChecking> inputChecking(new InputChecking);
	inputChecking->set_login(TCHAR_TO_UTF8(*text.ToString()));

	UMessageEncoder::Send(inputChecking.get(), false, true);
}

void URegWidgets::OnPasswordTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	wPasswordTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 4)
	{
		bPassOk = false;
		wPassImage->SetBrushFromTexture(denied_tex);
		wInfoBlock->SetText(FText::FromString("Error : Too short password"));
		return;
	}

	wInfoBlock->SetText(FText::FromString(" "));
	wPassImage->SetBrushFromTexture(accept_tex);
	bPassOk = true;
}

void URegWidgets::OnMailTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@._-");

	wMainTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 4)
	{
		bMailOk = false;
		wMailImage->SetBrushFromTexture(denied_tex);
		wInfoBlock->SetText(FText::FromString("Error : Too short mail"));
		return;
	}

	wInfoBlock->SetText(FText::FromString(" "));
	wMailImage->SetBrushFromTexture(empty_tex);

	std::shared_ptr<InputChecking> inputChecking(new InputChecking);
	inputChecking->set_mail(TCHAR_TO_UTF8(*text.ToString()));

	UMessageEncoder::Send(inputChecking.get(), false, true);
}

void URegWidgets::OnCaptchaTextChanged(const FText & text)
{
	std::string captcha_str(TCHAR_TO_UTF8(*wCaptchaTextBox->GetText().ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(captcha_str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	wCaptchaTextBox->SetText(FText::FromString(captcha_str.c_str()));

	if (captcha_str.length() < 5)
	{
		bCaptchaOk = false;
		wCaptchaCheckImage->SetBrushFromTexture(denied_tex);
		wInfoBlock->SetText(FText::FromString("Error : Too short captcha"));
		return;
	}

	wInfoBlock->SetText(FText::FromString(" "));
	wCaptchaCheckImage->SetBrushFromTexture(empty_tex);

	std::shared_ptr<InputChecking> inputChecking(new InputChecking);
	inputChecking->set_captcha(captcha_str);
	UMessageEncoder::Send(inputChecking.get(), false, true);
}

void URegWidgets::SingUpButtonClicked()
{
	if (bLoginOk && bPassOk && bMailOk && bCaptchaOk)
	{
		USpikyGameInstance::DifferentMix->RunWaitingScreen();

		std::shared_ptr<Registration> registration(new Registration);
		registration->set_login(TCHAR_TO_UTF8(*wLoginTextBox->GetText().ToString()));
		registration->set_mail(TCHAR_TO_UTF8(*wMainTextBox->GetText().ToString()));
		registration->set_captcha(TCHAR_TO_UTF8(*wCaptchaTextBox->GetText().ToString()));
		UMessageEncoder::Send(registration.get(), false, true);
	}
	else
	{
		wInfoBlock->SetText(FText::FromString("Error : Enter valid login/pass/mail/captcha"));
	}
}