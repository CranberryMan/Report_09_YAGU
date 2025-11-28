// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/yagu_GameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/yagu_PlayerController.h"

void Ayagu_GameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			Ayagu_PlayerController* ygPC = Cast<Ayagu_PlayerController>(PC);
			if (IsValid(ygPC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				ygPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
