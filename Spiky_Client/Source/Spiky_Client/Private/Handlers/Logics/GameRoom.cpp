// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "GameRoom.h"
#include "Descriptors.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "MainMenuWidgets.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"
#include "Runtime/UMG/Public/Components/HorizontalBoxSlot.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/ButtonSlot.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "RoomListUnit.h"
#include "GameRoomWidgets.h"
#include "GameRoomUserUnit.h"
#include "CreateRoomWidgets.h"

std::string UGameRoom::roomCreator = "";
std::string UGameRoom::roomName = "";

void UGameRoom::Handler(Room room)
{
	if (room.has_createroom())
	{
		USpikyGameInstance::DifferentMix->wMainMenuWidgets->AddRoom(NewRoom(room.createroom()));

		// If it's the creator then open the room, if not, just update the list in the main menu
		if (room.createroom().creator() == USpikyGameInstance::userLogin)
		{
			roomCreator = room.createroom().creator();
			roomName = room.createroom().roomname();
			USpikyGameInstance::DifferentMix->ShowGameRoom();
		}
	}
	else if (room.has_roomslistupdate())
	{
		if (!room.startgame())
		{
			// Remove the room from the list, return unallocated players to the menu
			DeleteRoom(room.roomslistupdate());
		}
		else
		{
			DeleteRoom(room.roomslistupdate().roomowner());
		}
	}
	else if (room.has_subscriberoom())
	{
		Subscribe(room.subscriberoom());
	}
	else if (room.has_roomupdate())
	{
		if (room.roomupdate().roomdescribe().GetReflection()->HasField(room.roomupdate().roomdescribe(), Descriptors::chat_room))
		{
			UChats * chats = NewObject<UChats>(UChats::StaticClass());
			chats->Handler(room.roomupdate().roomdescribe().chat(), "gameroom");
		}
		else
		{
			UpdateRoom(room.roomupdate());
		}
	}
}

void UGameRoom::Subscribe(SubscribeRoom subscribe) 
{
	if (subscribe.statecode() == 1) {
		GLog->Log("The room is full");
	}
	else if (subscribe.statecode() == 2) {
		GLog->Log("The player is already in the room");
	}
	else if (subscribe.has_roomdescribe()) {
		// Apply the description of the room
		roomCreator = subscribe.roomdescribe().creator();
		roomName = subscribe.roomdescribe().roomname(); // And other parameters if necessary

		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wFirstTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wSecondTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wChatScrollBox->ClearChildren();

		for (TeamPlayer e : subscribe.roomdescribe().team1())
		{
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->AddPlayer(e.player_name().c_str(), "team1");
		}
		for (TeamPlayer e : subscribe.roomdescribe().team2())
		{
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->AddPlayer(e.player_name().c_str(), "team2");
		}

		for (TeamPlayer e : subscribe.roomdescribe().undistributed())
		{
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->AddPlayer(e.player_name().c_str(), "undistributed");
		}

		UChats * chats = NewObject<UChats>(UChats::StaticClass());
		chats->Handler(subscribe.roomdescribe().chat(), "gameroom");

		// Open the room
		USpikyGameInstance::DifferentMix->wMainMenuWidgets->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wCreateRoomWidgets->SetVisibility(ESlateVisibility::Hidden);

		USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wMainMenuWidgets->bChatOpen = false;

		USpikyGameInstance::DifferentMix->wGameRoomWidgets->SetVisibility(ESlateVisibility::Visible);
		// Hide list management buttons
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wToFirstTeamButton->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wToSecondTeamButton->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wToUndistributedTeamButton->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wStartButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (subscribe.GetReflection()->HasField(subscribe, Descriptors::player_sub) && !subscribe.GetReflection()->HasField(subscribe, Descriptors::player_team))
	{
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->AddPlayer(subscribe.player().c_str(), "undistributed");
	}
	else if (!subscribe.subscribe())
	{
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->RemovePlayer(subscribe.player().c_str(), subscribe.team().c_str());

		if (USpikyGameInstance::userLogin == subscribe.player())
		{
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->wFirstTeamScrollBox->ClearChildren();
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->wSecondTeamScrollBox->ClearChildren();
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->ClearChildren();
			USpikyGameInstance::DifferentMix->wGameRoomWidgets->wChatScrollBox->ClearChildren();
			// Close the room
			USpikyGameInstance::DifferentMix->ShowMainMenuScreen();
		}
	}
}

void UGameRoom::UpdateRoom(RoomUpdate update)
{
	for (TeamPlayer e : update.roomdescribe().team1())
	{
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->RemoveAddPlayer(e.player_name().c_str(), "team1", update.targetteam().c_str());
	}

	for (TeamPlayer e : update.roomdescribe().team2())
	{
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->RemoveAddPlayer(e.player_name().c_str(), "team2", update.targetteam().c_str());
	}

	for (TeamPlayer e : update.roomdescribe().undistributed())
	{
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->RemoveAddPlayer(e.player_name().c_str(), "undistributed", update.targetteam().c_str());
	}
}

URoomListUnit* UGameRoom::NewRoom(CreateRoom room)
{
	URoomListUnit* button = NewObject<URoomListUnit>(URoomListUnit::StaticClass());
	UHorizontalBox* horBox = NewObject<UHorizontalBox>(UHorizontalBox::StaticClass());

	UTextBlock* wRoomName = NewObject<UTextBlock>(UTextBlock::StaticClass());
	UTextBlock* wMapName = NewObject<UTextBlock>(UTextBlock::StaticClass());
	UTextBlock* wCreator = NewObject<UTextBlock>(UTextBlock::StaticClass());
	UTextBlock* wGameTime = NewObject<UTextBlock>(UTextBlock::StaticClass());
	UTextBlock* wMaxPlayers = NewObject<UTextBlock>(UTextBlock::StaticClass());

	wRoomName->SetText(FText::FromString(TCHAR_TO_UTF8(*FString(room.roomname().c_str()))));
	wMapName->SetText(FText::FromString(TCHAR_TO_UTF8(*FString(room.mapname().c_str()))));
	wCreator->SetText(FText::FromString(TCHAR_TO_UTF8(*FString(room.creator().c_str()))));
	wGameTime->SetText(FText::FromString(TCHAR_TO_UTF8(*FString(room.gametime().c_str()))));
	wMaxPlayers->SetText(FText::FromString(TCHAR_TO_UTF8(*FString(room.maxplayers().c_str()))));

	UHorizontalBoxSlot* roomNameSlot = Cast<UHorizontalBoxSlot>(horBox->AddChild(wRoomName));
	UHorizontalBoxSlot* mapNameSlot = Cast<UHorizontalBoxSlot>(horBox->AddChild(wMapName));
	UHorizontalBoxSlot* creatorSlot = Cast<UHorizontalBoxSlot>(horBox->AddChild(wCreator));
	UHorizontalBoxSlot* gameTimeSlot = Cast<UHorizontalBoxSlot>(horBox->AddChild(wGameTime));
	UHorizontalBoxSlot* maxPlayersSlot = Cast<UHorizontalBoxSlot>(horBox->AddChild(wMaxPlayers));

	roomNameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	roomNameSlot->SetHorizontalAlignment(HAlign_Center);

	mapNameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	mapNameSlot->SetHorizontalAlignment(HAlign_Center);

	creatorSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	creatorSlot->SetHorizontalAlignment(HAlign_Center);

	gameTimeSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	gameTimeSlot->SetHorizontalAlignment(HAlign_Center);

	maxPlayersSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	maxPlayersSlot->SetHorizontalAlignment(HAlign_Center);

	UButtonSlot* buttonSlot = Cast<UButtonSlot>(button->AddChild(horBox));
	buttonSlot->SetHorizontalAlignment(HAlign_Fill);
	buttonSlot->SetVerticalAlignment(VAlign_Fill);

	return button;
}

void UGameRoom::DeleteRoom(RoomsListUpdate update)
{
	// Remove room from the list
	for (size_t i = 0; i < USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->GetChildrenCount(); i++)
	{
		URoomListUnit* listUnit = Cast<URoomListUnit>(USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->GetChildAt(i));
		UHorizontalBox* horBox = Cast<UHorizontalBox>(listUnit->GetChildAt(0));

		UTextBlock* wCreatorName = Cast<UTextBlock>(horBox->GetChildAt(2));

		if (wCreatorName->GetText().ToString() == update.roomowner().c_str())
		{
			USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->RemoveChildAt(i);
		}
	}

	// Return the players who were in this room to the main menu
	if (roomCreator == update.roomowner().c_str())
	{
		// Reset widget state for later use
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wFirstTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wSecondTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->ClearChildren();
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->wChatScrollBox->ClearChildren();
		UGameRoomUserUnit::select_players.clear();
		roomCreator = "";
		roomName = "";

		USpikyGameInstance::DifferentMix->wMainMenuWidgets->SetVisibility(ESlateVisibility::Visible);
		USpikyGameInstance::DifferentMix->wCreateRoomWidgets->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wGameRoomWidgets->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGameRoom::DeleteRoom(std::string name)
{
	// Remove room from the list
	for (size_t i = 0; i < USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->GetChildrenCount(); i++)
	{
		URoomListUnit* listUnit = Cast<URoomListUnit>(USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->GetChildAt(i));
		UHorizontalBox* horBox = Cast<UHorizontalBox>(listUnit->GetChildAt(0));

		UTextBlock* wCreatorName = Cast<UTextBlock>(horBox->GetChildAt(2));

		if (wCreatorName->GetText().ToString() == name.c_str())
		{
			USpikyGameInstance::DifferentMix->wMainMenuWidgets->wRoomsScrollBox->RemoveChildAt(i);
		}
	}

	// If we are in another room
	if (roomCreator == name)
	{
		// Return the players who were unallocated to the main menu
		for (size_t j = 0; j < USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->GetChildrenCount(); j++)
		{
			UGameRoomUserUnit * entity = Cast<UGameRoomUserUnit>(USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->GetChildAt(j));
			FString playerName = Cast<UTextBlock>(entity->GetChildAt(0))->GetText().ToString();

			// If the player is on this list and is not the creator
			if (playerName == USpikyGameInstance::userLogin.c_str() && playerName != roomCreator.c_str())
			{
				// Reset widget state for later use
				USpikyGameInstance::DifferentMix->wGameRoomWidgets->wFirstTeamScrollBox->ClearChildren();
				USpikyGameInstance::DifferentMix->wGameRoomWidgets->wSecondTeamScrollBox->ClearChildren();
				USpikyGameInstance::DifferentMix->wGameRoomWidgets->wUndistributedTeamScrollBox->ClearChildren();
				USpikyGameInstance::DifferentMix->wGameRoomWidgets->wChatScrollBox->ClearChildren();
				UGameRoomUserUnit::select_players.clear();
				roomCreator = "";
				roomName = "";

				USpikyGameInstance::DifferentMix->wMainMenuWidgets->SetVisibility(ESlateVisibility::Visible);
				USpikyGameInstance::DifferentMix->wCreateRoomWidgets->SetVisibility(ESlateVisibility::Hidden);
				USpikyGameInstance::DifferentMix->wGameRoomWidgets->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UGameRoom::StartGame()
{
}