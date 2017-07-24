// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include <vector>
#include "GameProcess.generated.h"

class GameData;
class PlayerPosition;

UCLASS()
class SPIKY_CLIENT_API UGameProcess : public UObject
{
	GENERATED_BODY()

public:

	void Handler(GameData gData);

	static GameData gameData;

	void UpdatePositions(PlayerPosition playerPosition);

	static std::vector<int64> pings;

	void ComputePing(GameData gData);

	void ComputeShot(GameData gData);
};
