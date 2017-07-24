// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpikyGameMode.generated.h"

/**
 * Game mod for non game screens
 */
UCLASS()
class SPIKY_CLIENT_API ASpikyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void TestSendUPDMessage();
	
};
