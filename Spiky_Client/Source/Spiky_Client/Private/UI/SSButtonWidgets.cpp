// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "SSButtonWidgets.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "SetServerWidgets.h"

void USSButtonWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wSettingsButton = Cast<UButton>(GetWidgetFromName(TEXT("SettingsButton")));
	wSettingsButton->OnClicked.AddDynamic(this, &USSButtonWidgets::SettingsButtonClicked);
}

void USSButtonWidgets::SettingsButtonClicked()
{
	if (USpikyGameInstance::DifferentMix->wServerSettings->GetVisibility() == ESlateVisibility::Hidden)
	{
		USpikyGameInstance::DifferentMix->wServerSettings->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		USpikyGameInstance::DifferentMix->wServerSettings->SetAddress();
		USpikyGameInstance::DifferentMix->wServerSettings->SetVisibility(ESlateVisibility::Hidden);
	}
}