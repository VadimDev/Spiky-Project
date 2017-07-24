// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Protobufs/GameModels.pb.h"
#include "MessageEncoder.h"

ATPSCharacter::ATPSCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ATPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ATPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::Fire);
}

void ATPSCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		FRotator Rotation = Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::Fire()
{
	GLog->Log("FIRE");
	
	APlayerController* controller = Cast<APlayerController>(GetController());

	FVector start = controller->PlayerCameraManager->GetCameraLocation();
	FVector end = start + controller->PlayerCameraManager->GetActorForwardVector() * 512;

	FCollisionQueryParams TraceParams(FName(TEXT("Trace")));
	TraceParams.bTraceComplex = true;

	FHitResult HitData(ForceInit);

	//Trace!
	GetWorld()->LineTraceSingleByChannel(HitData, start, end, ECC_Pawn, TraceParams);

	FVector startZ = FVector(start.X, start.Y, start.Z - 10);
	FVector endZ = FVector(end.X, end.Y, end.Z + 10);
	UKismetSystemLibrary::DrawDebugLine(this, startZ, endZ, FColor(255, 0, 0), 3.f, 1.f);

	std::shared_ptr<Shot> shot(new Shot);
	std::shared_ptr<Shot::Start> shotStart(new Shot::Start);
	std::shared_ptr<Shot::End> shotEnd(new Shot::End);

	shotStart->set_x(start.X);
	shotStart->set_y(start.Y);
	shotStart->set_z(start.Z);

	shotEnd->set_x(end.X);
	shotEnd->set_y(end.Y);
	shotEnd->set_z(end.Z);

	shot->set_allocated_start(shotStart.get());
	shot->set_allocated_end(shotEnd.get());

	if (IsValid(HitData.GetActor()))
		if (HitData.GetActor()->Tags.Num() > 0)
			shot->set_requestto(TCHAR_TO_UTF8(*HitData.GetActor()->Tags.Top().ToString()));

	shot->set_timestamp(USpikyGameInstance::DifferentMix->GetMS());

	std::shared_ptr<GameData> gameData(new GameData);
	gameData->set_allocated_shot(shot.get());

	// The beginning and the end of the shot, the time of hit in ms, the name of the player in which they dared
	UMessageEncoder::Send(gameData.get(), true, true);

	gameData->release_shot();
	shot->release_end();
	shot->release_start();
}