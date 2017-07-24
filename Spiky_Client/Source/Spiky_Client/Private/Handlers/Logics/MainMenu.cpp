// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MainMenu.h"
#include "Protobufs/MainMenuModels.pb.h"
#include "Descriptors.h"
#include "Chats.h"

void UMainMenu::Handler(MainMenu mainMenu)
{
	if (mainMenu.GetReflection()->HasField(mainMenu, Descriptors::chat_mm))
	{
		UChats * chats = NewObject<UChats>(UChats::StaticClass());
		chats->Handler(mainMenu.chat(), "mainmenu");
	}
}