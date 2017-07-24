// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "GameFramework/HUD.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "SpikyHUD.generated.h"

UCLASS()
class SPIKY_CLIENT_API ASpikyHUD : public AHUD
{
	GENERATED_BODY()
	
	ASpikyHUD();
	
private:
	/** Crosshair asset pointer */
	UTexture2D* CrosshairTex;

	virtual void DrawHUD() override;
	
};
