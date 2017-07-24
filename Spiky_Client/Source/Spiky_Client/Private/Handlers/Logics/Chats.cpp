// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "Chats.h"
#include "RichText.h"
#include "Descriptors.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "MainMenuChatWidgets.h"
#include "GameRoomWidgets.h"
#include "Protobufs/MainMenuModels.pb.h"

void UChats::Handler(Chat chat, FString type)
{
	if (type == "mainmenu")
	{
		if (chat.GetReflection()->HasField(chat, Descriptors::nameField_chat))
		{
			USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->NewMessage(RichMessage(chat));
		}
		else if (chat.messages_size() != 0)
		{
			for (ChatMessage e : chat.messages())
			{
				Chat chatMessage;
				chatMessage.set_time(e.time());
				chatMessage.set_name(e.name());
				chatMessage.set_text(e.text());

				USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->NewMessage(RichMessage(chatMessage));
			}
		}
	}
	else if (type == "gameroom")
	{
		if (chat.GetReflection()->HasField(chat, Descriptors::nameField_chat))
		{
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->NewMessage(RichMessage(chat));
		}
		else if (chat.messages_size() != 0)
		{
			for (ChatMessage e : chat.messages())
			{
				Chat chatMessage;
				chatMessage.set_time(e.time());
				chatMessage.set_name(e.name());
				chatMessage.set_text(e.text());

				USpikyGameInstance::DifferentMix->wGameRoomWidgets->NewMessage(RichMessage(chatMessage));
			}
		}
	}
}

URichText* UChats::RichMessage(Chat chat)
{
	URichText* richText = NewObject<URichText>(URichText::StaticClass());

	size_t seconds = (chat.time() / 1000) % 60;
	size_t minutes = ((chat.time() / (1000 * 60)) % 60);
	size_t hours = ((chat.time() / (1000 * 60 * 60)) % 24);

	FString h, m, s;

	if (hours < 10)
	{
		h = "0" + FString::FromInt(hours);
	}
	else {
		h = FString::FromInt(hours);
	}

	if (minutes < 10)
	{
		m = "0" + FString::FromInt(minutes);
	}
	else
	{
		m = FString::FromInt(minutes);
	}

	if (seconds < 10)
	{
		s = "0" + FString::FromInt(seconds);
	}
	else
	{
		s = FString::FromInt(seconds);
	}

	FString time = "<span color=\"#AEB5F9\">" + h + ":" + m + ":" + s + "</>";
	FString user = "<span color=\"#BDA05A\">" + FString(chat.name().c_str()) + ":" + "</>";
	FString text = chat.text().c_str();

	FString message = time + " " + user + " " + text;
	richText->SetText(FText::FromString(message));

	return richText;
}