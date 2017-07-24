// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Engine/GameInstance.h"
#include "VerificationServerGameInstance.generated.h"

UCLASS()
class VERIFICATIONSERVER_API UVerificationServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	virtual void Shutdown() override;	
};
