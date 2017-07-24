// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "CreateRoomWidgets.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Runtime/UMG/Public/Components/ComboBoxString.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "MessageEncoder.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "MainMenuWidgets.h"

void UCreateRoomWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wCreateRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("CreateRoomButton")));
	wCreateRoomButton->OnClicked.AddDynamic(this, &UCreateRoomWidgets::CreateRoomButtonClicked);

	wRoomNameTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("RoomName")));
	wRoomNameTextBox->OnTextChanged.AddDynamic(this, &UCreateRoomWidgets::OnRoomNameTextChanged);

	wMapComboBox = Cast<UComboBoxString>(GetWidgetFromName(TEXT("MapComboBox")));
	wGameTimeComboBox = Cast<UComboBoxString>(GetWidgetFromName(TEXT("TimeComboBox")));
	wPlayersNumberComboBox = Cast<UComboBoxString>(GetWidgetFromName(TEXT("PNComboBox")));
}

void UCreateRoomWidgets::CreateRoomButtonClicked()
{
	if (!bRoomNameOk)
	{
		USpikyGameInstance::DifferentMix->wMainMenuWidgets->ShowErrorMessage("To short room name");
		return;
	}

	USpikyGameInstance::DifferentMix->wMainMenuWidgets->bCreateRoomOpen = false;

	std::shared_ptr<CreateRoom> create_room(new CreateRoom);
	create_room->set_roomname(TCHAR_TO_UTF8(*wRoomNameTextBox->GetText().ToString()));
	create_room->set_mapname(TCHAR_TO_UTF8(*wMapComboBox->GetSelectedOption()));
	create_room->set_gametime(TCHAR_TO_UTF8(*wGameTimeComboBox->GetSelectedOption()));
	create_room->set_maxplayers(TCHAR_TO_UTF8(*wPlayersNumberComboBox->GetSelectedOption()));

	std::shared_ptr<Room> room(new Room);
	room->set_allocated_createroom(create_room.get());

	UMessageEncoder::Send(room.get(), true, true);

	room->release_createroom();
}

void UCreateRoomWidgets::OnRoomNameTextChanged(const FText & text)
{
	std::string str(TCHAR_TO_UTF8(*text.ToString()));

	USpikyGameInstance::DifferentMix->StringCleaner(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-");

	wRoomNameTextBox->SetText(FText::FromString(str.c_str()));

	if (str.length() < 3)
	{
		bRoomNameOk = false;

		USpikyGameInstance::DifferentMix->wMainMenuWidgets->ShowErrorMessage("To short room name");

		return;
	}

	bRoomNameOk = true;
}
