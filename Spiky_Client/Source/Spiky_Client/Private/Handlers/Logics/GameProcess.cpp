// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "GameProcess.h"
#include "Protobufs/GameModels.pb.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SpikyGameInstance.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"

GameData UGameProcess::gameData;
std::vector<int64> UGameProcess::pings;

void UGameProcess::Handler(GameData gData)
{
	if (gData.has_gameinitialstate())
	{
		gameData = gData;
		UGameplayStatics::OpenLevel(USpikyGameInstance::DifferentMix->GetWorld(), "Map1", false, "game=Spiky_Client.MapGameMode");
	}
	else if (gData.has_playerposition())
	{
		UpdatePositions(gData.playerposition());
	}
	else if (gData.has_shot())
	{
		ComputeShot(gData);
	}
	else if (gData.has_ping())
	{
		ComputePing(gData);
	}
}

void UGameProcess::ComputeShot(GameData gData)
{
	// todo When hit and when not
	// Draw a laser shot
	FVector start = FVector(gData.shot().start().x(), gData.shot().start().y(), gData.shot().start().z());
	FVector end = FVector(gData.shot().end().x(), gData.shot().end().y(), gData.shot().end().z());
	UKismetSystemLibrary::DrawDebugLine(this, start, end, FColor(255, 0, 0), 10.f, 1.f);
}

void UGameProcess::UpdatePositions(PlayerPosition playerPosition)
{
	for (TObjectIterator<ATPSCharacter> actorItr; actorItr; ++actorItr)
	{
		if (actorItr->ActorHasTag(playerPosition.playername().c_str()))
		{
			actorItr->SetActorLocation(FVector(playerPosition.loc().x(), playerPosition.loc().y(), playerPosition.loc().z()));
			actorItr->SetActorRotation(FRotator(playerPosition.rot().pitch(), playerPosition.rot().yaw(), playerPosition.rot().roll()));
		}
	}
}

void UGameProcess::ComputePing(GameData gData)
{
	int64 ping = USpikyGameInstance::DifferentMix->GetMS() - gData.ping().time();

	if (pings.size() < 5)
	{
		pings.push_back(ping);
	}
	else
	{
		int64 avr_ping = 0;

		for (int64 p : pings)
		{
			avr_ping += p;
		}

		avr_ping /= 5;
		pings.clear();

		FString str = "Ping: " + FString::FromInt(avr_ping) + " ms";
		USpikyGameInstance::DifferentMix->wPingFpsWidgets->wPing->SetText(FText::FromString(str));
	}
}