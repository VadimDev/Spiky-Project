// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include "MessageDecoder.generated.h"

class Wrapper;

UCLASS()
class SPIKY_CLIENT_API UMessageDecoder : public UObject
{
	GENERATED_BODY()

public:
	
	void SendProtoToDecoder(Wrapper * wrapper);

};
