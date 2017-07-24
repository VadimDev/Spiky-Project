// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MainMenuWidgets.generated.h"

class UTextBlock;
class UButton;
class UScrollBox;
class URoomListUnit;

UCLASS()
class SPIKY_CLIENT_API UMainMenuWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	UButton* wOpenChatButton = nullptr;
	UTextBlock* wPlayerName = nullptr;
	UTextBlock* wInfoBlock = nullptr;

	UButton* wCreateRoomButton = nullptr;

	UFUNCTION()
	void OpenChatButtonClicked();

	UFUNCTION()
	void CreateRoomButtonClicked();

	FTimerHandle MessageTimerHandle;
	void HideErrorMessage();
	void ShowErrorMessage(FString msg);

	UScrollBox * wRoomsScrollBox = nullptr;

	bool bChatOpen = false;
	bool bCreateRoomOpen = false;

	void AddRoom(URoomListUnit* room);

};
