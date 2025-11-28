// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "yagu_Notification.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class YAGU_API Uyagu_Notification : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetChatLog(const FText& NewMessage);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetTurnNoti(const FText& NewMessage);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetTurnSecond(const FText& NewMessage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotiList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TurnSecond;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TurnNoti;
};
