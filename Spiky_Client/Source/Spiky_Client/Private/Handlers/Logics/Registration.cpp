// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Registration.h"
#include "Protobufs/RegLogModels.pb.h"
#include "Descriptors.h"
#include "Crypto.h"
#include "MessageEncoder.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "RegWidgets.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"

void URegistration::Handler(Registration registration)
{
	if (registration.GetReflection()->HasField(registration, Descriptors::publicKey_reg))
	{
		// create public, private key
		keys keys_set = UCrypto::Generate_KeysSet_DH();

		std::shared_ptr<Registration> reg(new Registration);
		reg->set_publickey(keys_set.pubKey);
		UMessageEncoder::Send(reg.get(), false, true); // send public key to server

		USpikyGameInstance::secretKey = UCrypto::Generate_SecretKey_DH(registration.publickey());
	}
	else if (registration.GetReflection()->HasField(registration, Descriptors::stateCode_reg))
	{
		if (registration.statecode() == 0) // error while registration
		{
			USpikyGameInstance::DifferentMix->wRegistration->wInfoBlock->SetText(FText::FromString("Unknown error try to enter everything again"));
			USpikyGameInstance::DifferentMix->wRegistration->wLoginTextBox->SetText(FText::FromString(""));
			USpikyGameInstance::DifferentMix->wRegistration->wPasswordTextBox->SetText(FText::FromString(""));
			USpikyGameInstance::DifferentMix->wRegistration->wMainTextBox->SetText(FText::FromString(""));
			USpikyGameInstance::DifferentMix->wRegistration->wCaptchaTextBox->SetText(FText::FromString(""));
			USpikyGameInstance::DifferentMix->wRegistration->ReloadCaptchaClicked();
			USpikyGameInstance::DifferentMix->StopWaitingScreen();
		}
		else if (registration.statecode() == 1) // successful secret key generation
		{
			GLog->Log("Successful secret key generation");

			std::shared_ptr<Registration> reg(new Registration);
			reg->set_login(TCHAR_TO_UTF8(*USpikyGameInstance::DifferentMix->wRegistration->wLoginTextBox->GetText().ToString()));
			std::string hash_str = TCHAR_TO_UTF8(*USpikyGameInstance::DifferentMix->wRegistration->wPasswordTextBox->GetText().ToString());
			reg->set_hash(UCrypto::SHA256(&hash_str, hash_str.length()));
			reg->set_mail(TCHAR_TO_UTF8(*USpikyGameInstance::DifferentMix->wRegistration->wMainTextBox->GetText().ToString()));

			UMessageEncoder::Send(reg.get(), true, true);
		}
	}
}