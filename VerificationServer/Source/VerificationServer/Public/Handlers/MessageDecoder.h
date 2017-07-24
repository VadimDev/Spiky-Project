// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "MessageDecoder.generated.h"

class GameData;

UCLASS()
class VERIFICATIONSERVER_API UMessageDecoder : public UObject
{
	GENERATED_BODY()

	virtual class UWorld* GetWorld() const override;

public:
	
	void SendProtoToDecoder(GameData* gameData);

};
