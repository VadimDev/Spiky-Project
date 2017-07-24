// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/UMG/Public/Components/Button.h"
#include <vector>
#include "GameRoomUserUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegateGameRoom);

UCLASS()
class SPIKY_CLIENT_API UGameRoomUserUnit: public UButton
{
	GENERATED_BODY()

public:

	UGameRoomUserUnit();

	UPROPERTY()
	FClickDelegateGameRoom click;

	UFUNCTION()
	void OnClick();

	bool bSelect = false;

	static std::vector<std::pair<FString, FString>> select_players;
};