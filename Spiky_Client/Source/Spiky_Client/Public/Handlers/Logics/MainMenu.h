// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "MainMenu.generated.h"

class MainMenu;

UCLASS()
class SPIKY_CLIENT_API UMainMenu : public UObject
{
	GENERATED_BODY()

public:

	void Handler(MainMenu mainMenu);
};
