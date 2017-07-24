// Copyright (c) 2017, Vadim Petrov - MIT License

#pragma once

#include "Spiky_Client.h"

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "WSWidgets.generated.h"

UCLASS()
class SPIKY_CLIENT_API UWSWidgets : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	FTimerHandle MessageTimerHandle;

	bool once = true;

	UImage * wGear1 = nullptr;
	UImage * wGear2 = nullptr;

	FWidgetTransform transform1;
	FWidgetTransform transform2;
	void GearsAnim();
};
