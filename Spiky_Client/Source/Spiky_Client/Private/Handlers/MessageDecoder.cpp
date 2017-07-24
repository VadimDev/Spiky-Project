// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MessageDecoder.h"
#include "Protobufs/MessageModels.pb.h"
#include "InputChecking.h"
#include "Registration.h"
#include "Crypto.h"
#include "Login.h"
#include "Descriptors.h"
#include "SpikyGameInstance.h"
#include "InitialState.h"
#include "MainMenu.h"
#include "Protobufs/MainMenuModels.pb.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "GameRoom.h"
#include "GameProcess.h"

void UMessageDecoder::SendProtoToDecoder(Wrapper * wrapper)
{
	if (wrapper->has_inputchecking())
	{
		UInputChecking * inputChecking = NewObject<UInputChecking>(UInputChecking::StaticClass());
		inputChecking->Handler(wrapper->inputchecking());
	}
	else if (wrapper->has_registration())
	{
		URegistration * registration = NewObject<URegistration>(URegistration::StaticClass());
		registration->Handler(wrapper->registration());
	}
	else if (wrapper->has_login())
	{
		ULogin * login = NewObject<ULogin>(ULogin::StaticClass());
		login->Handler(wrapper->login());
	}
	else if (wrapper->has_cryptogramwrapper())
	{
		if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::registration_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().registration(), USpikyGameInstance::secretKey);

			Registration registration;
			registration.ParseFromArray(source.c_str(), source.length());

			URegistration * reg = NewObject<URegistration>(URegistration::StaticClass());
			reg->Handler(registration);
		}
		else if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::login_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().login(), USpikyGameInstance::secretKey);

			Login login;
			login.ParseFromArray(source.c_str(), source.length());

			ULogin * log = NewObject<ULogin>(ULogin::StaticClass());
			log->Handler(login);
		}
		else if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::initialState_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().initialstate(), USpikyGameInstance::secretKey);

			InitialState is;
			is.ParseFromArray(source.c_str(), source.length());

			UInitialState * initialState = NewObject<UInitialState>(UInitialState::StaticClass());
			initialState->Handler(is);
		}
		else if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::mainMenu_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().mainmenu(), USpikyGameInstance::secretKey);

			MainMenu mainMenu;
			mainMenu.ParseFromArray(source.c_str(), source.length());

			UMainMenu * menu = NewObject<UMainMenu>(UMainMenu::StaticClass());
			menu->Handler(mainMenu);
		}
		else if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::room_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().room(), USpikyGameInstance::secretKey);

			Room room;
			room.ParseFromArray(source.c_str(), source.length());

			UGameRoom * gameRoom = NewObject<UGameRoom>(UGameRoom::StaticClass());
			gameRoom->Handler(room);
		}
		else if (wrapper->cryptogramwrapper().GetReflection()->HasField(wrapper->cryptogramwrapper(), Descriptors::gameModels_cw))
		{
			std::string source = UCrypto::Decrypt(wrapper->cryptogramwrapper().gamemodels(), USpikyGameInstance::secretKey);

			GameData gameData;
			gameData.ParseFromArray(source.c_str(), source.length());

			UGameProcess * gameProcess = NewObject<UGameProcess>(UGameProcess::StaticClass());
			gameProcess->Handler(gameData);
		}
	}
}