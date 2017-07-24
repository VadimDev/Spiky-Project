// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Login.h"
#include "Protobufs/RegLogModels.pb.h"
#include "SpikyGameInstance.h"
#include "LoginWidgets.h"
#include "Descriptors.h"
#include "Crypto.h"
#include "MessageEncoder.h"
#include "DifferentMix.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

void ULogin::Handler(Login login)
{
	if (login.GetReflection()->HasField(login, Descriptors::publicKey_log))
	{
		USpikyGameInstance::secretKey = UCrypto::Generate_SecretKey_DH(login.publickey());

		std::shared_ptr<Login> login_proto(new Login);
		login_proto->set_mail(ULoginWidgets::mail);
		login_proto->set_hash(UCrypto::SHA256(&ULoginWidgets::password, ULoginWidgets::password.length()));

		UMessageEncoder::Send(login_proto.get(), true, true);
	}
	if (login.GetReflection()->HasField(login, Descriptors::stateCode_log))
	{
		USpikyGameInstance::DifferentMix->StopWaitingScreen();
		USpikyGameInstance::DifferentMix->wLoginScreen->wInfoBlock->SetText(FText::FromString("Incorrect login or password"));

		GLog->Log(FString("Session ID: NULL"));
		GLog->Log(FString("Login: NULL"));
	}
}