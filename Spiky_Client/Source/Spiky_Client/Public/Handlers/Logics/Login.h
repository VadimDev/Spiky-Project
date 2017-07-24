// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Login.generated.h"

class Login;

UCLASS()
class SPIKY_CLIENT_API ULogin : public UObject
{
	GENERATED_BODY()

public:

	void Handler(Login login);

};
