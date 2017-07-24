// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/Actor.h"
#include "MechActor.generated.h"

UCLASS()
class SPIKY_CLIENT_API AMechActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AMechActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshComponent)
	UStaticMeshComponent* MeshComponent;
};
