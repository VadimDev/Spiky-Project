// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UCLASS()
class SPIKY_CLIENT_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATPSCharacter();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void MoveForward(float Val);

	UFUNCTION()
	void MoveRight(float Val);
	
	void Fire();
};
