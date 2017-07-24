// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

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
	APlayerController* controller = Cast<APlayerController>(GetController());

	FVector start = controller->PlayerCameraManager->GetCameraLocation();
	FVector end = start + controller->PlayerCameraManager->GetActorForwardVector() * 512;

	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true);
	TraceParams.bTraceComplex = true;

	FHitResult HitData(ForceInit);

	//Trace!
	GetWorld()->LineTraceSingleByChannel(HitData, start, end, ECC_Pawn, TraceParams);

	FVector startZ = FVector(start.X, start.Y, start.Z - 10);
	FVector endZ = FVector(end.X, end.Y, end.Z + 10);
	UKismetSystemLibrary::DrawDebugLine(this, startZ, endZ, FColor(255, 0, 0), 3.f, 1.f);

	if (IsValid(HitData.GetActor()))
	{
		if (HitData.GetActor()->Tags.Num() > 0) {
			GLog->Log(HitData.GetActor()->GetName());
			GLog->Log(HitData.GetActor()->Tags.Top().ToString());
			GLog->Log(HitData.BoneName.ToString());
		}
	}
}