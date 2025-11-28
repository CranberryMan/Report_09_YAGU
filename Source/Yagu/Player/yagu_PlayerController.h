// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "yagu_PlayerController.generated.h"

class Uyagu_ChatInput;
class Ayagu_gamemodebase;
class Uyagu_Notification;
/**
 * 
 */
UCLASS()
class YAGU_API Ayagu_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	Ayagu_PlayerController();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString, bool isGuessNumber);

	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
	
	UFUNCTION(Client, Reliable)
	void setNotiText(const FText& InNotiText);

	bool IsGuessNumberString(const FString& InNumberString);
	void IncreaseGuessCount();
	
	UFUNCTION(Client, Reliable)
	void resetGuessCount();

	UFUNCTION(Client, Reliable)
	void setTurnText(const FText& InNotiText);

	UFUNCTION(Client, Reliable)
	void setTurnCount(const FText& InNotiText);
	
	UPROPERTY(Replicated)
	bool turnOff;

	UFUNCTION(Client, Reliable)
	void ClientRPCIncreaseGuessCount();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<Uyagu_ChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<Uyagu_ChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<Uyagu_Notification> NotiWidgetClass;

	UPROPERTY()
	TObjectPtr<Uyagu_Notification> NotiWidgetInstance;

	FString ChatMessageString;


	FText messageMaker(const FString& msg);
	TArray<FText> messageHistory;
	const int32 MaxHistorySize = 10;
};
