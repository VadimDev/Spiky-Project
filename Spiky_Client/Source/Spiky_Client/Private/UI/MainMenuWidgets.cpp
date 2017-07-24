// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MainMenuWidgets.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Components/ScrollBoxSlot.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Protobufs/MainMenuModels.pb.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "MainMenuChatWidgets.h"
#include "MessageEncoder.h"
#include "CreateRoomWidgets.h"
#include "RoomListUnit.h"

void UMainMenuWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wOpenChatButton = Cast<UButton>(GetWidgetFromName(TEXT("OpenChatButton")));
	wOpenChatButton->OnClicked.AddDynamic(this, &UMainMenuWidgets::OpenChatButtonClicked);

	wCreateRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("CreateRoomButton")));
	wCreateRoomButton->OnClicked.AddDynamic(this, &UMainMenuWidgets::CreateRoomButtonClicked);

	wPlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName")));

	wInfoBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("InfoBox")));

	wRoomsScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("RoomsScrollBox")));
}

void UMainMenuWidgets::OpenChatButtonClicked()
{
	if (!bChatOpen)
	{
		bChatOpen = true;
		USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		std::shared_ptr<Chat> chat(new Chat);
		chat->set_subscribe(true);

		std::shared_ptr<MainMenu> mainMenu(new MainMenu);
		mainMenu->set_allocated_chat(chat.get());

		UMessageEncoder::Send(mainMenu.get(), true, true);

		mainMenu->release_chat();
	}
	else
	{
		bChatOpen = false;
		USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->SetVisibility(ESlateVisibility::Hidden);
		USpikyGameInstance::DifferentMix->wMainMenuChatWidgets->wChatScrollBox->ClearChildren();

		std::shared_ptr<Chat> chat(new Chat);
		chat->set_subscribe(false);

		std::shared_ptr<MainMenu> mainMenu(new MainMenu);
		mainMenu->set_allocated_chat(chat.get());

		UMessageEncoder::Send(mainMenu.get(), true, true);

		mainMenu->release_chat();
	}
}

void UMainMenuWidgets::CreateRoomButtonClicked()
{
	if (!bCreateRoomOpen)
	{
		bCreateRoomOpen = true;
		USpikyGameInstance::DifferentMix->wCreateRoomWidgets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		bCreateRoomOpen = false;
		USpikyGameInstance::DifferentMix->wCreateRoomWidgets->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuWidgets::AddRoom(URoomListUnit* room)
{
	UScrollBoxSlot* gameRoomSlot = Cast<UScrollBoxSlot>(wRoomsScrollBox->AddChild(room));
	gameRoomSlot->SetHorizontalAlignment(HAlign_Fill);
}

void UMainMenuWidgets::HideErrorMessage()
{
	wInfoBlock->SetText(FText::FromString(" "));
}

void UMainMenuWidgets::ShowErrorMessage(FString msg)
{
	wInfoBlock->SetText(FText::FromString(*msg));

	GetWorld()->GetTimerManager().ClearTimer(MessageTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, this, &UMainMenuWidgets::HideErrorMessage, 2.f, false);
}
