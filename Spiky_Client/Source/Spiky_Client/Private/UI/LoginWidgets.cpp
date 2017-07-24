// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "LoginWidgets.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "SocketObject.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "RegWidgets.h"
#include "Crypto.h"
#include "Protobufs/RegLogModels.pb.h"
#include "SetServerWidgets.h"
#include "MessageEncoder.h"

std::string ULoginWidgets::mail = "";
std::string ULoginWidgets::password = "";

void ULoginWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wSingUpButton = Cast<UButton>(GetWidgetFromName(TEXT("SingUpButton")));
	wSingUpButton->OnClicked.AddDynamic(this, &ULoginWidgets::SingUpButtonClicked);

	wLoginButton = Cast<UButton>(GetWidgetFromName(TEXT("LoginButton")));
	wLoginButton->OnClicked.AddDynamic(this, &ULoginWidgets::LoginButtonClicked);

	wInfoBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBlock")));

	wMailTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("MailBox")));
	wMailTextBox->OnTextChanged.AddDynamic(this, &ULoginWidgets::OnMailTextChanged);

	wPasswordTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("PasswordBox")));
	wPasswordTextBox->OnTextChanged.AddDynamic(this, &ULoginWidgets::OnPasswordTextChanged);
}

void ULoginWidgets::LoginButtonClicked()
{
	if (bMailOk && bPassOk && USocketObject::bIsConnection)
	{
		GLog->Log("ULoginWidgets::LoginButtonClicked()");

		USpikyGameInstance::DifferentMix->RunWaitingScreen();

		keys keys_set = UCrypto::Generate_KeysSet_DH();

		std::shared_ptr<Login> login_proto(new Login);
		login_proto->set_publickey(keys_set.pubKey);
		UMessageEncoder::Send(login_proto.get(), false, true);
	}
	else
	{
		if (!USocketObject::bIsConnection)
		{
			ShowErrorMessage("No connection");
		}
		else if (!bMailOk && !bPassOk)
		{
			ShowErrorMessage("Incorrect mail and password");
		}
		else if (!bMailOk)
		{
			ShowErrorMessage("Incorrect mail");
		}
		else if (!bPassOk)
		{
			ShowErrorMessage("Incorrect password");
		}
	}
}

void ULoginWidgets::SettingsButtonClicked()
{
	USpikyGameInstance::DifferentMix->wServerSettings->SetVisibility(ESlateVisibility::Visible);
}

void ULoginWidgets::SingUpButtonClicked()
{
	if (USocketObject::bIsConnection)
	{
		USpikyGameInstance::DifferentMix->ShowRegistrationScreen();
		USpikyGameInstance::DifferentMix->wRegistration->ReloadCaptchaClicked();
	}
	else
	{
		ShowErrorMessage("No connection");
	}
}

void ULoginWidgets::HideErrorMessage()
{
	wInfoBlock->SetText(FText::FromString(" "));
}

void ULoginWidgets::ShowErrorMessage(FString msg)
{
	wInfoBlock->SetText(FText::FromString(*msg));

	GetWorld()->GetTimerManager().ClearTimer(MessageTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, this, &ULoginWidgets::HideErrorMessage, 1.2f, false);
}

void ULoginWidgets::OnMailTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@._-");

	wMailTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 4)
	{
		bMailOk = false;
		ShowErrorMessage("Too short mail");
		return;
	}

	HideErrorMessage();
	bMailOk = true;
	mail = str;
}

void ULoginWidgets::OnPasswordTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	wPasswordTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 4)
	{
		bPassOk = false;
		ShowErrorMessage("Too short password");
		return;
	}

	HideErrorMessage();
	bPassOk = true;
	password = str;
}