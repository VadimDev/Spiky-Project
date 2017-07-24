// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "MechActor.h"
#include "Runtime/Engine/Classes/GameFramework/RotatingMovementComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

AMechActor::AMechActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	MeshComponent->SetMobility(EComponentMobility::Movable);

	RootComponent = MeshComponent;

	URotatingMovementComponent* RotatingMovement = ObjectInitializer.CreateDefaultSubobject<URotatingMovementComponent>(this, TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0, 80, 0); // speed
	RotatingMovement->PivotTranslation = FVector(0, 0, 0); // point

	RotatingMovement->SetUpdatedComponent(GetRootComponent());
}