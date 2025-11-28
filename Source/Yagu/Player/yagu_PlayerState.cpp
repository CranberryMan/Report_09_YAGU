// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/yagu_PlayerState.h"
#include "Net/UnrealNetwork.h"

Ayagu_PlayerState::Ayagu_PlayerState() :
	FS_Name(TEXT("NoName")),
	CurrentGuessCount(0),
	MaxGuessCount(3),
	isPlayer(false)
{
	bReplicates = true;

}

void Ayagu_PlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(Ayagu_PlayerState, FS_Name);
	DOREPLIFETIME(Ayagu_PlayerState, CurrentGuessCount);
	DOREPLIFETIME(Ayagu_PlayerState, MaxGuessCount);
	DOREPLIFETIME(Ayagu_PlayerState, isPlayer);	
}

FString Ayagu_PlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = FS_Name + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}