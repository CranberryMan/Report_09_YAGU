// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "yagu_gamemodebase.generated.h"

class Ayagu_PlayerController;

/**
 * 
 */
UCLASS()
class YAGU_API Ayagu_gamemodebase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	void PrintChatMessageString(Ayagu_PlayerController* InChattingPlayerController, const FString& InChatMessageString, bool IsGuessNumberString);


	FString GenerateSecretNumber();
	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);


	void ResetGame();
	void JudgeGame(Ayagu_PlayerController* InChattingPlayerController, int InStrikeCount);

	void gameStart();

	void turnStart();
	void turnCountTimer();

protected:
	FString SecretNumberString;
	TArray<TObjectPtr<Ayagu_PlayerController>> AllPlayerControllers;
	TArray<TObjectPtr<Ayagu_PlayerController>> gamePlayerControllers;
	FTimerHandle GameStartTimerHandle;
	FTimerHandle GameCountTimerHandle;
private:
	bool bIsGameStarted = false;
	int32 TimerCount = 0;
	int32 nowTurn = 0;

};
