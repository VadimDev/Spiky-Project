// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "MessageEncoder.generated.h"

class GameData;

UCLASS()
class VERIFICATIONSERVER_API UMessageEncoder : public UObject
{
	GENERATED_BODY()

public:

	static bool Send(GameData* gameData);


};
