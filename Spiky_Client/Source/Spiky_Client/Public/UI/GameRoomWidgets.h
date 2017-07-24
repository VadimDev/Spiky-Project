// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "GameRoomWidgets.generated.h"

class UScrollBox;
class UButton;
class UMultiLineEditableTextBox;
class URichText;

UCLASS()
class SPIKY_CLIENT_API UGameRoomWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	UScrollBox* wUndistributedTeamScrollBox = nullptr;
	UScrollBox* wFirstTeamScrollBox = nullptr;
	UScrollBox* wSecondTeamScrollBox = nullptr;

	UButton* wToFirstTeamButton = nullptr;
	UButton* wToSecondTeamButton = nullptr;
	UButton* wToUndistributedTeamButton = nullptr;

	UButton* wStartButton = nullptr;
	UButton* wGameInfoButton = nullptr;
	UButton* wCloseButton = nullptr;
	UButton* wEnterButton = nullptr;

	UScrollBox * wChatScrollBox = nullptr;
	UMultiLineEditableTextBox* wChatTextBox = nullptr;

	UFUNCTION()
	void StartButtonClicked();

	UFUNCTION()
	void GameInfoButtonClicked();

	UFUNCTION()
	void ToFirstTeamClicked();

	UFUNCTION()
	void ToSecondTeamClicked();

	UFUNCTION()
	void ToUndistributedClicked();

	UFUNCTION()
	void CloseButtonClicked();

	UFUNCTION()
	void EnterButtonClicked();

	void NewMessage(URichText* richText);

	void ScrollToEnd();

	void AddPlayer(FString player, FString team);
	void RemovePlayer(FString player, FString team);
	void RemoveAddPlayer(FString player, FString sourceTeam, FString targetTeam);
	void TeamSelector(FString team);
};
