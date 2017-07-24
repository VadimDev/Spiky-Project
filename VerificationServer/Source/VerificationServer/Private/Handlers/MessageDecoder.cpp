// Copyright (c) 2017, Vadim Petrov - MIT License

#include "VerificationServer.h"
#include "MessageDecoder.h"
#include "Protobufs/GameModels.pb.h"
#include "MessageEncoder.h"
#include "VerificationServerGameMode.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

void UMessageDecoder::SendProtoToDecoder(GameData* gameData)
{
	if (gameData->has_shot())
	{
		GLog->Log("shot");

		FVector location = FVector(
			gameData->shot().playerposition().loc().x(), 
			gameData->shot().playerposition().loc().y(), 
			gameData->shot().playerposition().loc().z()
		);

		FRotator rotation = FRotator(
			gameData->shot().playerposition().rot().pitch(),
			gameData->shot().playerposition().rot().yaw(),
			gameData->shot().playerposition().rot().roll()
		);

		AVerificationServerGameMode* gameMode = (AVerificationServerGameMode*)GetWorld()->GetAuthGameMode();

		// Place on the map
		auto character = GetWorld()->SpawnActor<ATPSCharacter>(gameMode->pawnObject->GetClass(), location, rotation);
		character->Tags.Add(gameData->shot().requestto().c_str());

		//GLog->Log(character->GetActorLocation().ToString());

		// Check the hit
		FVector start = FVector(
			gameData->shot().start().x(),
			gameData->shot().start().y(),
			gameData->shot().start().z()
		);

		FVector end = FVector(
			gameData->shot().end().x(),
			gameData->shot().end().y(),
			gameData->shot().end().z()
		);

		FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true);
		TraceParams.bTraceComplex = true;

		FHitResult HitData(ForceInit);

		GetWorld()->LineTraceSingleByChannel(HitData, start, end, ECC_Pawn, TraceParams);
		UKismetSystemLibrary::DrawDebugLine(this, start, end, FColor(255, 0, 0), 60.f, 1.f);

		if (IsValid(HitData.GetActor()))
		{
			if (HitData.GetActor()->Tags.Num() > 0) {
				GLog->Log(HitData.GetActor()->GetName());
				GLog->Log(HitData.GetActor()->Tags.Top().ToString());
				GLog->Log(HitData.BoneName.ToString());

				// If the name of the goal coincides with the name of the one in which hit
				if (gameData->shot().requestto().c_str() == HitData.GetActor()->Tags.Top().ToString())
				{
					gameData->mutable_shot()->set_result_hitstate(true);
					gameData->mutable_shot()->set_result_bonename(TCHAR_TO_UTF8(*HitData.BoneName.ToString()));

					UMessageEncoder::Send(gameData);
				}
				else
				{
					gameData->mutable_shot()->set_result_hitstate(false);
					UMessageEncoder::Send(gameData);
				}
			}
		}
		else
		{
			gameData->mutable_shot()->set_result_hitstate(false);
			UMessageEncoder::Send(gameData);
		}	
	}
}

class UWorld* UMessageDecoder::GetWorld() const
{
	return AVerificationServerGameMode::world;
}