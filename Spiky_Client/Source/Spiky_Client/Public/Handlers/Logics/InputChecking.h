// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "InputChecking.generated.h"

class InputChecking;

UCLASS()
class SPIKY_CLIENT_API UInputChecking : public UObject
{
	GENERATED_BODY()

public:

	void Handler(const InputChecking inputChecking);

};
