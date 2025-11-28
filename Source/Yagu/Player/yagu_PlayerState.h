// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "yagu_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class YAGU_API Ayagu_PlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	Ayagu_PlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	FString GetPlayerInfoString();

public:
	UPROPERTY(Replicated)
	FString FS_Name;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	UPROPERTY(Replicated)
	bool isPlayer;

};
