// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "yagu_ChatInput.generated.h"

class UEditableTextBox;
class UTextBlock;

/**
 * 
 */
UCLASS()
class YAGU_API Uyagu_ChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	void SetFocusOnInput();

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetChatLog(const FText& NewMessage);

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ETB_ChatInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatList;
	
};
