// Copyright (c) 2017, Vadim Petrov - MIT License

#include "Spiky_Client.h"
#include "InputChecking.h"
#include "Descriptors.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "SpikyGameInstance.h"
#include "DifferentMix.h"
#include "RegWidgets.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Protobufs/RegLogModels.pb.h"

void UInputChecking::Handler(const InputChecking inputChecking)
{
	if (inputChecking.GetReflection()->HasField(inputChecking, Descriptors::captchaDataField_ich))
	{
		UTexture2D * tex = USpikyGameInstance::DifferentMix->CreateTexture(inputChecking.captchadata(), true);
		USpikyGameInstance::DifferentMix->wRegistration->wCaptchaImage->SetBrushFromTexture(tex);
	}
	else if (inputChecking.GetReflection()->HasField(inputChecking, Descriptors::loginCheckStatus_ich))
	{
		if (inputChecking.logincheckstatus())
		{
			USpikyGameInstance::DifferentMix->wRegistration->wLoginImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->accept_tex);
			USpikyGameInstance::DifferentMix->wRegistration->bLoginOk = true;
		}
		else
		{
			USpikyGameInstance::DifferentMix->wRegistration->wLoginImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->denied_tex);
		}
	}
	else if (inputChecking.GetReflection()->HasField(inputChecking, Descriptors::mailCheckStatus_ich))
	{
		if (inputChecking.mailcheckstatus())
		{
			USpikyGameInstance::DifferentMix->wRegistration->wMailImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->accept_tex);
			USpikyGameInstance::DifferentMix->wRegistration->bMailOk = true;
		}
		else
		{
			USpikyGameInstance::DifferentMix->wRegistration->wMailImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->denied_tex);
		}
	}
	else if (inputChecking.GetReflection()->HasField(inputChecking, Descriptors::captchaCheckStatus_ich))
	{
		if (inputChecking.captchacheckstatus())
		{
			USpikyGameInstance::DifferentMix->wRegistration->wCaptchaCheckImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->accept_tex);
			USpikyGameInstance::DifferentMix->wRegistration->bCaptchaOk = true;
		}
		else
		{
			USpikyGameInstance::DifferentMix->wRegistration->bCaptchaOk = false;
			USpikyGameInstance::DifferentMix->wRegistration->wCaptchaCheckImage->SetBrushFromTexture(USpikyGameInstance::DifferentMix->wRegistration->denied_tex);
		}
	}		
}
