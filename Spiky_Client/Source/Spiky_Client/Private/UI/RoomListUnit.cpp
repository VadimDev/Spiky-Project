// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "RoomListUnit.h"
#include "Protobufs/GameRoomModels.pb.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "MessageEncoder.h"

URoomListUnit::URoomListUnit()
{
	OnClicked.AddDynamic(this, &URoomListUnit::OnClick);
}

void URoomListUnit::OnClick()
{
	std::shared_ptr<Room> room(new Room);

	UHorizontalBox* horBox = Cast<UHorizontalBox>(GetChildAt(0));
	UTextBlock* textBlock = Cast<UTextBlock>(horBox->GetChildAt(0));

	std::shared_ptr<SubscribeRoom> subscribe(new SubscribeRoom);
	subscribe->set_subscribe(true);
	subscribe->set_roomname(TCHAR_TO_UTF8(*textBlock->GetText().ToString()));

	room->set_allocated_subscriberoom(subscribe.get());

	UMessageEncoder::Send(room.get(), true, true);

	room->release_subscriberoom();
}