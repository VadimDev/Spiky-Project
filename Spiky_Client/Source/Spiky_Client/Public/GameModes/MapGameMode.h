// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MapGameMode.generated.h"

class ATPSCharacter;

UCLASS()
class SPIKY_CLIENT_API AMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AMapGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	ATPSCharacter* pawnObject;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	FTimerHandle UpdateLocationTimerHandle;
	void SendLocation();

	FVector old_position, position;
	FRotator old_rotation, rotation;

	FTimerHandle PingTimerHandle;
	void SendPing();

	FTimerHandle FPSTimerHandle;
	void ComputeFrameRate();
	int32 fps = 0;
};
