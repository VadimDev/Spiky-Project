// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MainMenuChatWidgets.h"
#include "Runtime/UMG/Public/Components/ScrollBox.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/MultiLineEditableTextBox.h"
#include "Protobufs/MainMenuModels.pb.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "MessageEncoder.h"
#include "RichText.h"
#include "Runtime/UMG/Public/Components/ScrollBoxSlot.h"

void UMainMenuChatWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wEnterButton = Cast<UButton>(GetWidgetFromName(TEXT("EnterButton")));
	wEnterButton->OnClicked.AddDynamic(this, &UMainMenuChatWidgets::EnterButtonClicked);

	wChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));

	wChatTextBox = Cast<UMultiLineEditableTextBox>(GetWidgetFromName(TEXT("ChatTextBox")));
}

void UMainMenuChatWidgets::EnterButtonClicked()
{
	if (wChatTextBox->GetText().ToString() == "") return;

	std::shared_ptr<Chat> chat(new Chat);

	chat->set_text(TCHAR_TO_UTF8(*wChatTextBox->GetText().ToString()));

	wChatTextBox->SetText(FText::FromString(""));

	std::shared_ptr<MainMenu> mainMenu(new MainMenu);
	mainMenu->set_allocated_chat(chat.get());

	UMessageEncoder::Send(mainMenu.get(), true, true);

	mainMenu->release_chat();
}

void UMainMenuChatWidgets::NewMessage(URichText* richText)
{
	UScrollBoxSlot* richTextSlot = Cast<UScrollBoxSlot>(wChatScrollBox->AddChild(richText));
	richTextSlot->SetPadding(FMargin(10, 5, 10, 5));
	wChatScrollBox->ScrollToEnd();

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UMainMenuChatWidgets::ScrollToEnd, .01f, false);
}

void UMainMenuChatWidgets::ScrollToEnd()
{
	wChatScrollBox->ScrollToEnd();
}