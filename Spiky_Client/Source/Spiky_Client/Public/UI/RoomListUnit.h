// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Spiky_Client.h"

#include "Runtime/UMG/Public/Components/Button.h"
#include "RoomListUnit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

UCLASS()
class SPIKY_CLIENT_API URoomListUnit: public UButton
{
	GENERATED_BODY()

public:

	URoomListUnit();

	UPROPERTY()
	FClickDelegate click;

	UFUNCTION()
	void OnClick();
};
