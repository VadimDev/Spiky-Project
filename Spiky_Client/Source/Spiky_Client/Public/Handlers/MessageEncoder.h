// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include <google/protobuf/message.h>
#include "MessageEncoder.generated.h"

UCLASS()
class SPIKY_CLIENT_API UMessageEncoder : public UObject
{
	GENERATED_BODY()

public:

	static bool Send(google::protobuf::Message * message, bool bCrypt, bool bTCP);

};
