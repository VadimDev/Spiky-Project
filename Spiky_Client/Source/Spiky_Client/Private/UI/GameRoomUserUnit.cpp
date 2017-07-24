// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "GameRoomUserUnit.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include <algorithm>
#include "GameRoom.h"

std::vector<std::pair<FString, FString>> UGameRoomUserUnit::select_players;

UGameRoomUserUnit::UGameRoomUserUnit()
{
	OnClicked.AddDynamic(this, &UGameRoomUserUnit::OnClick);
}

void UGameRoomUserUnit::OnClick()
{
	// The creator can not select items from the list
	if (UGameRoom::roomCreator != USpikyGameInstance::userLogin) return;

	UTextBlock* textBlock = Cast<UTextBlock>(GetChildAt(0));

	FSlateBrush greenBackgroundImage;
	greenBackgroundImage.SetResourceObject((UObject*)(USpikyGameInstance::DifferentMix->GreenImage));

	FSlateBrush emptyBackgroundImage;
	emptyBackgroundImage.SetResourceObject((UObject*)(USpikyGameInstance::DifferentMix->EmptyImage));

	FString player_name = textBlock->GetText().ToString();;
	FString team_name = GetParent()->GetName();

	std::pair<FString, FString> item = std::make_pair(player_name, team_name);

	if (!bSelect)
	{
		bSelect = true;

		WidgetStyle.Pressed = greenBackgroundImage;
		WidgetStyle.Hovered = greenBackgroundImage;
		WidgetStyle.Normal  = greenBackgroundImage;

		select_players.push_back(item);
	}
	else
	{
		bSelect = false;

		WidgetStyle.Pressed = emptyBackgroundImage;
		WidgetStyle.Hovered = emptyBackgroundImage;
		WidgetStyle.Normal  = emptyBackgroundImage;

		auto it = std::find(select_players.begin(), select_players.end(), item);

		if (it == select_players.end()) 
		{ // not in vector 
		}
		else
		{
			auto index = std::distance(select_players.begin(), it);
			select_players.erase(select_players.begin() + index);
		}
	}

	GLog->Log(FString::FromInt(select_players.size()));
}
