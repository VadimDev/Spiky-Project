// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "GameRoomWidgets.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Components/ScrollBoxSlot.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/MultiLineEditableTextBox.h"
#include "GameRoomUserUnit.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "GameRoom.h"
#include "MessageEncoder.h"
#include "MainMenuWidgets.h"
#include "CreateRoomWidgets.h"
#include "RichText.h"
#include "Runtime/Engine/Public/TimerManager.h"

void UGameRoomWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wFirstTeamScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("team1")));
	wSecondTeamScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("team2")));
	wUndistributedTeamScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("undistributed")));

	wToFirstTeamButton = Cast<UButton>(GetWidgetFromName(TEXT("ToFTButton")));
	wToFirstTeamButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::ToFirstTeamClicked);

	wToSecondTeamButton = Cast<UButton>(GetWidgetFromName(TEXT("ToSTButton")));
	wToSecondTeamButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::ToSecondTeamClicked);

	wToUndistributedTeamButton = Cast<UButton>(GetWidgetFromName(TEXT("ToUndistributedButton")));
	wToUndistributedTeamButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::ToUndistributedClicked);

	wCloseButton = Cast<UButton>(GetWidgetFromName(TEXT("CloseButton")));
	wCloseButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::CloseButtonClicked);

	wEnterButton = Cast<UButton>(GetWidgetFromName(TEXT("EnterButton")));
	wEnterButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::EnterButtonClicked);

	wChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));
	wChatTextBox = Cast<UMultiLineEditableTextBox>(GetWidgetFromName(TEXT("ChatTextBox")));

	wStartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));
	wStartButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::StartButtonClicked);

	wGameInfoButton = Cast<UButton>(GetWidgetFromName(TEXT("GameInfoButton")));
	wGameInfoButton->OnClicked.AddDynamic(this, &UGameRoomWidgets::GameInfoButtonClicked);
}

void UGameRoomWidgets::AddPlayer(FString player, FString team)
{
	UGameRoomUserUnit* button = NewObject<UGameRoomUserUnit>(this, UGameRoomUserUnit::StaticClass());
	UTextBlock* textBlock = NewObject<UTextBlock>(this, UTextBlock::StaticClass());

	textBlock->SetText(FText::FromString(player));
	button->AddChild(textBlock);

	FSlateBrush emptyBackgroundImage;
	emptyBackgroundImage.SetResourceObject((UObject*)(USpikyGameInstance::DifferentMix->EmptyImage));
	button->WidgetStyle.Normal = emptyBackgroundImage;
	button->WidgetStyle.Hovered = emptyBackgroundImage;
	button->WidgetStyle.Pressed = emptyBackgroundImage;

	UScrollBoxSlot* buttonSlot = nullptr;

	if (team == "team1")
	{
		buttonSlot = Cast<UScrollBoxSlot>(wFirstTeamScrollBox->AddChild(button));
	}
	else if (team == "team2")
	{
		buttonSlot = Cast<UScrollBoxSlot>(wSecondTeamScrollBox->AddChild(button));
	}
	else if (team == "undistributed")
	{
		buttonSlot = Cast<UScrollBoxSlot>(wUndistributedTeamScrollBox->AddChild(button));
	}

	buttonSlot->SetHorizontalAlignment(HAlign_Fill);
}

void UGameRoomWidgets::RemovePlayer(FString player, FString team)
{
	UScrollBox* targetScrollBox = nullptr;

	if (team == "team1")
	{
		targetScrollBox = wFirstTeamScrollBox;
	}
	else if (team == "team2")
	{
		targetScrollBox = wSecondTeamScrollBox;
	}
	else if (team == "undistributed")
	{
		targetScrollBox = wUndistributedTeamScrollBox;
	}

	for (size_t i = 0; i < targetScrollBox->GetChildrenCount(); i++)
	{
		UGameRoomUserUnit* button = Cast<UGameRoomUserUnit>(targetScrollBox->GetChildAt(i));
		UTextBlock* textBlock = Cast<UTextBlock>(button->GetChildAt(0));
		if (textBlock->GetText().ToString() == player) targetScrollBox->RemoveChildAt(i);
	}

	// Remove from the list of selected players, if any
	auto it = UGameRoomUserUnit::select_players.begin();
	// Iterate on the selected players in the list
	while (it != UGameRoomUserUnit::select_players.end())
	{
		if (it->first == player)
		{
			it = UGameRoomUserUnit::select_players.erase(it);
		}
		else
		{
			++it;
		}
		GLog->Log(FString::FromInt(UGameRoomUserUnit::select_players.size()));
	}
}

void UGameRoomWidgets::RemoveAddPlayer(FString player, FString sourceTeam, FString targetTeam)
{
	// Removed the selection from the items if they are in the target list
	UScrollBox * targetScrollBox = nullptr;

	if (targetTeam == "team1") targetScrollBox = wFirstTeamScrollBox;
	if (targetTeam == "team2") targetScrollBox = wSecondTeamScrollBox;
	if (targetTeam == "undistributed") targetScrollBox = wUndistributedTeamScrollBox;

	for (size_t i = 0; i < targetScrollBox->GetChildrenCount(); i++)
	{
		UGameRoomUserUnit * entity = Cast<UGameRoomUserUnit>(targetScrollBox->GetChildAt(i));
		FString str = Cast<UTextBlock>(entity->GetChildAt(0))->GetText().ToString();

		for (auto const& e : UGameRoomUserUnit::select_players)
			if (str == e.first) entity->OnClick();
	}

	if (targetTeam == "team1" && sourceTeam != targetTeam)
	{
		RemovePlayer(player, sourceTeam);
		AddPlayer(player, targetTeam);
	}

	if (targetTeam == "team2" && sourceTeam != targetTeam)
	{
		RemovePlayer(player, sourceTeam);
		AddPlayer(player, targetTeam);
	}

	if (targetTeam == "undistributed" && sourceTeam != targetTeam)
	{
		RemovePlayer(player, sourceTeam);
		AddPlayer(player, targetTeam);
	}

	// Remove from the list of selected players, if any
	auto it = UGameRoomUserUnit::select_players.begin();

	while (it != UGameRoomUserUnit::select_players.end())
	{
		if (it->first == player)
		{
			it = UGameRoomUserUnit::select_players.erase(it);
		}
		else
		{
			++it;
		}
		GLog->Log(FString::FromInt(UGameRoomUserUnit::select_players.size()));
	}
}

void UGameRoomWidgets::TeamSelector(FString team)
{
	std::string targetTeam = TCHAR_TO_UTF8(*team);
	// Send the list of selected players to the server
	std::shared_ptr<RoomDescribe> roomDescribe(new RoomDescribe);

	for (auto s : UGameRoomUserUnit::select_players)
	{
		if (s.second == "undistributed")
		{
			TeamPlayer* teamPlayer = roomDescribe->add_undistributed();
			teamPlayer->set_player_name(TCHAR_TO_UTF8(*s.first));
		}
		else if (s.second == "team1")
		{
			TeamPlayer* teamPlayer = roomDescribe->add_team1();
			teamPlayer->set_player_name(TCHAR_TO_UTF8(*s.first));
		}
		else if (s.second == "team2")
		{
			TeamPlayer* teamPlayer = roomDescribe->add_team2();
			teamPlayer->set_player_name(TCHAR_TO_UTF8(*s.first));
		}
	}

	std::shared_ptr<RoomUpdate> roomUpdate(new RoomUpdate);
	roomUpdate->set_allocated_roomdescribe(roomDescribe.get());
	roomUpdate->set_targetteam(targetTeam);
	roomUpdate->set_roomname(UGameRoom::roomName);

	std::shared_ptr<Room> room(new Room);
	room->set_allocated_roomupdate(roomUpdate.get());

	UMessageEncoder::Send(room.get(), true, true);

	room->release_roomupdate();
	roomUpdate->release_roomdescribe();
}

void UGameRoomWidgets::StartButtonClicked()
{
	std::shared_ptr<Room> room(new Room);
	room->set_startgame(true);
	room->set_roomname(UGameRoom::roomName);
	UMessageEncoder::Send(room.get(), true, true);
}

void UGameRoomWidgets::GameInfoButtonClicked()
{
	// TODO
}

void UGameRoomWidgets::ToFirstTeamClicked()
{
	TeamSelector("team1");
}

void UGameRoomWidgets::ToUndistributedClicked()
{
	TeamSelector("undistributed");
}

void UGameRoomWidgets::ToSecondTeamClicked()
{
	TeamSelector("team2");
}

void UGameRoomWidgets::CloseButtonClicked()
{
	if (UGameRoom::roomCreator != USpikyGameInstance::userLogin)
	{
		// Reset widget state for later use
		wFirstTeamScrollBox->ClearChildren();
		wSecondTeamScrollBox->ClearChildren();
		wUndistributedTeamScrollBox->ClearChildren();
		wChatScrollBox->ClearChildren();
		// Return the player to the main menu
		USpikyGameInstance::DifferentMix->ShowMainMenuScreen();
		// Retire from the room
		std::shared_ptr<SubscribeRoom> unsubscribe(new SubscribeRoom);
		unsubscribe->set_subscribe(false);
		unsubscribe->set_roomname(UGameRoom::roomName);

		std::shared_ptr<Room> room(new Room);
		room->set_allocated_subscriberoom(unsubscribe.get());

		UMessageEncoder::Send(room.get(), true, true);
		room->release_subscriberoom();
	}
	else
	{
		std::shared_ptr<RoomsListUpdate> update(new RoomsListUpdate);
		update->set_deleteroom(true);

		std::shared_ptr<Room> room(new Room);
		room->set_allocated_roomslistupdate(update.get());

		UMessageEncoder::Send(room.get(), true, true);
		room->release_roomslistupdate();
	}
}

void UGameRoomWidgets::EnterButtonClicked()
{
	if (wChatTextBox->GetText().ToString() == "") return;

	std::shared_ptr<Room> room(new Room);
	std::shared_ptr<RoomUpdate> roomUpdate(new RoomUpdate);
	std::shared_ptr<RoomDescribe> roomDescribe(new RoomDescribe);
	std::shared_ptr<Chat> chat(new Chat);

	chat->set_text(TCHAR_TO_UTF8(*wChatTextBox->GetText().ToString()));
	wChatTextBox->SetText(FText::FromString(""));

	roomDescribe->set_allocated_chat(chat.get());
	roomUpdate->set_allocated_roomdescribe(roomDescribe.get());
	roomUpdate->set_roomname(UGameRoom::roomName);
	room->set_allocated_roomupdate(roomUpdate.get());

	UMessageEncoder::Send(room.get(), true, true);

	room->release_roomupdate();
	roomUpdate->release_roomdescribe();
	roomDescribe->release_chat();
}

void UGameRoomWidgets::NewMessage(URichText* richText)
{
	UScrollBoxSlot* richTextSlot = Cast<UScrollBoxSlot>(wChatScrollBox->AddChild(richText));
	richTextSlot->SetPadding(FMargin(10, 5, 10, 5));
	wChatScrollBox->ScrollToEnd();

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UGameRoomWidgets::ScrollToEnd, .01f, false);
}

void UGameRoomWidgets::ScrollToEnd()
{
	wChatScrollBox->ScrollToEnd();
}
