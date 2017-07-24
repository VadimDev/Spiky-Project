// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "SetServerWidgets.h"
#include "SocketObject.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"

void USetServerWidgets::NativeConstruct()
{
	Super::NativeConstruct();

	wAddressBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("AddressBox")));
	wPortBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("PortBox")));

	// default value
	uint32 OutIP;
	USocketObject::tcp_address->GetIp(OutIP);

	// Return ip the normal form
	FString ip = FString::Printf(TEXT("%d.%d.%d.%d"), 0xff & (OutIP >> 24), 0xff & (OutIP >> 16), 0xff & (OutIP >> 8), 0xff & OutIP);

	wAddressBox->SetText(FText::FromString(ip));
	wPortBox->SetText(FText::FromString(FString::FromInt(USocketObject::tcp_address->GetPort())));
}

void USetServerWidgets::SetAddress()
{
	uint32 OutIP;
	USocketObject::tcp_address->GetIp(OutIP);

	// Return ip the normal form
	FString oldIP = FString::Printf(TEXT("%d.%d.%d.%d"), 0xff & (OutIP >> 24), 0xff & (OutIP >> 16), 0xff & (OutIP >> 8), 0xff & OutIP);
	FString oldPort = FString::FromInt(USocketObject::tcp_address->GetPort());

	// Pick up data when close
	FIPv4Address serverIP;
	FIPv4Address::Parse(wAddressBox->GetText().ToString(), serverIP);
	int32 serverPort = FCString::Atoi(*(wPortBox->GetText().ToString()));

	FString newIP = serverIP.ToString();
	FString newPort = FString::FromInt(serverPort);

	GLog->Log(newIP + " " + newPort);

	// If the new input is different from the old one
	if (!oldIP.Equals(*newIP, ESearchCase::IgnoreCase) || !oldPort.Equals(*newPort, ESearchCase::IgnoreCase))
	{
		GLog->Log("Address change");
		USocketObject::tcp_address->SetIp(serverIP.Value);
		USocketObject::tcp_address->SetPort(FCString::Atoi(*(wPortBox->GetText().ToString())));
		USocketObject::Reconnect();
	}
}

