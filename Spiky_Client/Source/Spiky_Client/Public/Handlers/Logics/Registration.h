// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "Registration.generated.h"

class Registration;

UCLASS()
class SPIKY_CLIENT_API URegistration : public UObject
{
	GENERATED_BODY()

public:

	void Handler(Registration registration);

};
