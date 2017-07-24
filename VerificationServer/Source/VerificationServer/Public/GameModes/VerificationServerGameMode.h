// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/GameModeBase.h"
#include "TPSCharacter.h"
#include "VerificationServerGameMode.generated.h"

UCLASS()
class VERIFICATIONSERVER_API AVerificationServerGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AVerificationServerGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

public:

	ATPSCharacter* pawnObject;

	static UWorld* world;

};
