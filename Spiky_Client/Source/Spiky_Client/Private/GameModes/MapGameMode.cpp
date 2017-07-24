// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MapGameMode.h"
#include "SpikyGameInstance.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TPSCharacter.h"
#include "Protobufs/GameModels.pb.h"
#include "GameProcess.h"
#include "DifferentMix.h"
#include "MessageEncoder.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "SpikyHUD.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "PingFpsWidgets.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

AMapGameMode::AMapGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<ATPSCharacter> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/Pawns/TPSCharacter_BP.TPSCharacter_BP_C'"));

	if (PlayerPawnObject.Class != NULL)
	{
		pawnObject = PlayerPawnObject.Class->GetDefaultObject<ATPSCharacter>();
	}

	HUDClass = ASpikyHUD::StaticClass();
}

void AMapGameMode::BeginPlay()
{
	Super::BeginPlay();

	GLog->Log("AMapGameMode::BeginPlay()");

	USpikyGameInstance* gameInstance = Cast<USpikyGameInstance>(GetWorld()->GetGameInstance());
	gameInstance->DifferentMixInit(GetWorld());

	// Set the initial values of all other players
	for (::Player p : UGameProcess::gameData.gameinitialstate().player())
	{
		if (p.player_name() != USpikyGameInstance::userLogin)
		{
			FVector pos = FVector(p.playerposition().loc().x(), p.playerposition().loc().y(), p.playerposition().loc().z());
			auto character = GetWorld()->SpawnActor<ATPSCharacter>(pawnObject->GetClass(), FTransform(pos));

			character->Tags.Add(p.player_name().c_str());
		}	
	}

	gameInstance->DifferentMix->wPingFpsWidgets->SetVisibility(ESlateVisibility::Visible);

	// We send the player's position 20 times per second /10 .1 /20 .05 / 30 .03
	GetWorld()->GetTimerManager().SetTimer(UpdateLocationTimerHandle, this, &AMapGameMode::SendLocation, .05f, true);

	// Send the packet with the sending time five times per second
	GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, this, &AMapGameMode::SendPing, .25f, true);

	// Calculate frame rate (fps)
	GetWorld()->GetTimerManager().SetTimer(FPSTimerHandle, this, &AMapGameMode::ComputeFrameRate, 1.f, true);
}

void AMapGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AMapGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fps++;
}

AActor* AMapGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	FVector pos;

	for (::Player p : UGameProcess::gameData.gameinitialstate().player())
	{
		if (p.player_name() == USpikyGameInstance::userLogin)
		{
			pos = FVector(p.playerposition().loc().x(), p.playerposition().loc().y(), p.playerposition().loc().z());
		}
	}

	ATPSCharacter* playerCharacter = GetWorld()->SpawnActor<ATPSCharacter>(pawnObject->GetClass(), FTransform(pos));
	GetWorld()->GetFirstPlayerController()->Possess(playerCharacter);

	playerCharacter->Tags.Add(USpikyGameInstance::userLogin.c_str());

	return playerCharacter;
}

void AMapGameMode::SendLocation()
{
	ATPSCharacter* character = Cast<ATPSCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	position = character->GetActorLocation();
	rotation = character->GetActorRotation();

	// Do not send anything if there are no changes
	if (position.Equals(old_position) && rotation.Equals(old_rotation)) return;

	old_position = position;
	old_rotation = rotation;

	std::shared_ptr<PlayerPosition> playerPosition(new PlayerPosition);
	std::shared_ptr<PlayerPosition::Location> playerLocation(new PlayerPosition::Location);
	std::shared_ptr<PlayerPosition::Rotation> playerRotation(new PlayerPosition::Rotation);

	playerLocation->set_x(position.X);
	playerLocation->set_y(position.Y);
	playerLocation->set_z(position.Z);

	playerRotation->set_pitch(rotation.Pitch);
	playerRotation->set_roll(rotation.Roll);
	playerRotation->set_yaw(rotation.Yaw);

	playerPosition->set_allocated_loc(playerLocation.get());
	playerPosition->set_allocated_rot(playerRotation.get());

	playerPosition->set_timestamp(USpikyGameInstance::DifferentMix->GetMS());

	std::shared_ptr<GameData> gameData(new GameData);
	gameData->set_allocated_playerposition(playerPosition.get());

	UMessageEncoder::Send(gameData.get(), true, true);

	gameData->release_playerposition();
	playerPosition->release_rot();
	playerPosition->release_loc();
}

void AMapGameMode::SendPing()
{
	std::shared_ptr<Ping> ping(new Ping);

	ping->set_time(USpikyGameInstance::DifferentMix->GetMS());

	std::shared_ptr<GameData> gameData(new GameData);
	gameData->set_allocated_ping(ping.get());

	UMessageEncoder::Send(gameData.get(), true, true);

	gameData->release_ping();
}

void AMapGameMode::ComputeFrameRate()
{
	USpikyGameInstance::DifferentMix->wPingFpsWidgets->wFps->SetText(FText::FromString(FString::FromInt(fps) + " FPS"));
	fps = 0;
}