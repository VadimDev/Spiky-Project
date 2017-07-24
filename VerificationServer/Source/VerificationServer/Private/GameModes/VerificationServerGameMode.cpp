// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "VerificationServerGameMode.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

UWorld* AVerificationServerGameMode::world = nullptr;

AVerificationServerGameMode::AVerificationServerGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<ATPSCharacter> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/TPSCharacter_bp.TPSCharacter_BP_C'"));

	if (PlayerPawnObject.Class != NULL)
	{
		//DefaultPawnClass = PlayerPawnObject.Class;
		pawnObject = PlayerPawnObject.Class->GetDefaultObject<ATPSCharacter>();
	}
}

void AVerificationServerGameMode::BeginPlay()
{
	Super::BeginPlay();

	/*
	auto character = GetWorld()->SpawnActor<ATPSCharacter>(pawnObject->GetClass());

	if (IsValid(character))
	{
		character->Tags.Add("Mike");
	}
	*/

	world = GetWorld();
}
