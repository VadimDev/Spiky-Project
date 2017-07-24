// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UCLASS()
class VERIFICATIONSERVER_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Val);

	UFUNCTION()
	void MoveRight(float Val);

	void Fire();
};
