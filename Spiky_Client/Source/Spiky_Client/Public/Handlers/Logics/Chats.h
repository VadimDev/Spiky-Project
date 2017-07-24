// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Chats.generated.h"

class URichText;
class Chat;

UCLASS()
class SPIKY_CLIENT_API UChats : public UObject
{
	GENERATED_BODY()

public:

	void Handler(Chat chat, FString type);

	URichText* RichMessage(Chat chat);
};
