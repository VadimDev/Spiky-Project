// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <string>
#include "SpikyGameInstance.generated.h"

class UDifferentMix;

/**
 * Storage of global, level independent objects
 */
UCLASS()
class SPIKY_CLIENT_API USpikyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	virtual void Init() override;

	virtual void Shutdown() override;

public:

	static UWorld* world;

	void DifferentMixInit(UWorld* the_world);

	static UDifferentMix * DifferentMix;

	static std::string secretKey;
	static std::string sessionId;
	static std::string userLogin;
};
