// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "InitialState.generated.h"

class InitialState;

UCLASS()
class SPIKY_CLIENT_API UInitialState : public UObject
{
	GENERATED_BODY()

public:

	void Handler(InitialState initialState);

};
