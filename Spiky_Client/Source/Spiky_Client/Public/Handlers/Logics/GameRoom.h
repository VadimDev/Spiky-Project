// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Runtime/CoreUObject/Public/UObject/Object.h"
#include <string>
#include "GameRoom.generated.h"

class Room;
class CreateRoom;
class URoomListUnit;
class RoomsListUpdate;
class SubscribeRoom;
class RoomUpdate;

UCLASS()
class SPIKY_CLIENT_API UGameRoom : public UObject
{
	GENERATED_BODY()

public:

	void Handler(Room room);

	static URoomListUnit* NewRoom(CreateRoom room);

	static std::string roomCreator;
	static std::string roomName;

	void DeleteRoom(RoomsListUpdate room);
	void DeleteRoom(std::string name);
	void Subscribe(SubscribeRoom subscribe);
	void UpdateRoom(RoomUpdate update);
	void StartGame();
};
