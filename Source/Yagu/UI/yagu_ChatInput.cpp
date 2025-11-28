// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/yagu_ChatInput.h"
#include "Components/TextBlock.h"

#include "Components/EditableTextBox.h"
#include "Player/yagu_PlayerController.h"


void Uyagu_ChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ETB_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		ETB_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void Uyagu_ChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (ETB_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
	{
		ETB_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void Uyagu_ChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// 아무것도 입력 안했는데 엔터 연타했을 때 로직 수행안하기 위함
		if (Text.IsEmpty())
		{
			return;
		}

		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController))
		{
			Ayagu_PlayerController* OwningCXPlayerController = Cast<Ayagu_PlayerController>(OwningPlayerController);
			if (IsValid(OwningCXPlayerController))
			{
				OwningCXPlayerController->SetChatMessageString(Text.ToString());
				ETB_ChatInput->SetText(FText());
			}

			// 커밋 후에도 포커스가 남아있게 하기 위해서
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SetFocusOnInput);
				// 또는 짧은 시간을 지정:
				//FTimerHandle FocusTimerHandle;
				//World->GetTimerManager().SetTimer(FocusTimerHandle, this, &ThisClass::SetFocusOnInput, 0.01f, false);
			}
		}
	}
}

void Uyagu_ChatInput::SetFocusOnInput()
{
	if (IsValid(ETB_ChatInput))
	{
		ETB_ChatInput->SetKeyboardFocus();
	}
}

void Uyagu_ChatInput::SetChatLog(const FText& NewMessage)
{
	if (IsValid(ChatList))
	{
		ChatList->SetText(NewMessage);
	}
}
