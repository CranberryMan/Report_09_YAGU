// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/yagu_PlayerController.h"
#include "UI/yagu_ChatInput.h"
#include "UI/yagu_Notification.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/yagu_gamemodebase.h"
#include "Player/yagu_PlayerState.h"
#include "Net/UnrealNetwork.h"

Ayagu_PlayerController::Ayagu_PlayerController() :
	turnOff(false)
{
	bReplicates = true;
}

void Ayagu_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}


	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass))
	{
		ChatInputWidgetInstance = CreateWidget<Uyagu_ChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
	if (IsValid(NotiWidgetClass))
	{
		NotiWidgetInstance = CreateWidget<Uyagu_Notification>(this, NotiWidgetClass);
		if (IsValid(NotiWidgetInstance))
		{
			NotiWidgetInstance->SetTurnSecond(FText::FromString(TEXT("")));
			NotiWidgetInstance->SetTurnNoti(FText::FromString(TEXT("")));
			NotiWidgetInstance->AddToViewport();
		}
	}

	/// 게임모드 init
	//AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	//if (IsValid(GM))
	//{
	//	Ayagu_gamemodebase* ygGM = Cast<Ayagu_gamemodebase>(GM);
	//	if (IsValid(ygGM))
	//	{
	//		this->yaguGM = ygGM;
	//	}
	//	
	//}
	bShowMouseCursor = true;
}

void Ayagu_PlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Ayagu_PlayerController, NotificationText);
	DOREPLIFETIME(Ayagu_PlayerController, turnOff);
}

void Ayagu_PlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;
	// PrintChatMessageString(ChatMessageString);
	if (IsLocalController())
	{
		// ServerRPCPrintChatMessageString(InChatMessageString);
		Ayagu_PlayerState* ygPS = GetPlayerState<Ayagu_PlayerState>();
		if (IsValid(ygPS))
		{
			UE_LOG(LogTemp, Warning, TEXT("%d"), turnOff);

			if (ygPS->isPlayer && !turnOff)
			{
				// FString CombinedMessageString = ygPS->FS_Name + TEXT(": ") + InChatMessageString;
				bool isGuessNumber = IsGuessNumberString(InChatMessageString);
				if (isGuessNumber)
				{
					IncreaseGuessCount();
				}
				FString CombinedMessageString = ygPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
				ServerRPCPrintChatMessageString(CombinedMessageString, isGuessNumber);
			}
			else if (ygPS->isPlayer && turnOff)
			{
				FText a = FText::FromString("Your Turn is Off");
				setNotiText(a);
				FString CombinedMessageString = ygPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
				ServerRPCPrintChatMessageString(CombinedMessageString, false);
			}
			else
			{
				FString CombinedMessageString = ygPS->FS_Name + TEXT(": ") + InChatMessageString;
				ServerRPCPrintChatMessageString(CombinedMessageString, false);
			}

		}
	}
}

void Ayagu_PlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	/// 차후 위젯에 UI 갱신하는 식으로 바꿔보자
	UE_LOG(LogTemp, Warning, TEXT("%s"), *InChatMessageString);
	// UKismetSystemLibrary::PrintString(this, InChatMessageString, true, true, FLinearColor::Red, 5.0f);	

	if (IsValid(ChatInputWidgetInstance))
	{
		FText a = messageMaker(InChatMessageString);
		ChatInputWidgetInstance->SetChatLog(a);
	}

}

// 클라이언트에서 처리하기 위해 RPC로 만들었다.
void Ayagu_PlayerController::setNotiText_Implementation(const FText& InNotiText)
{
	NotificationText = InNotiText;
	if (IsValid(NotiWidgetInstance))
	{
		NotiWidgetInstance->SetChatLog(NotificationText);
	}
}

FText Ayagu_PlayerController::messageMaker(const FString& msg)
{

	messageHistory.Add(FText::FromString(msg));
	if(messageHistory.Num() > MaxHistorySize)
	{
		messageHistory.RemoveAt(0);
	}

	FText rtrText;
	for(int i = 0; i < messageHistory.Num(); i++)
	{
		rtrText = FText::FromString(rtrText.ToString() + messageHistory[i].ToString() + "\n");
	}

	return rtrText;
}

void Ayagu_PlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void Ayagu_PlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString, bool isGuessNumber)
{
	//for (TActorIterator<Ayagu_PlayerController> It(GetWorld()); It; ++It)
	//{
	//	Ayagu_PlayerController* CXPlayerController = *It;
	//	if (IsValid(CXPlayerController))
	//	{
	//		CXPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
	//	}
	//}

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM))
	{
		Ayagu_gamemodebase* ygGM = Cast<Ayagu_gamemodebase>(GM);
		if (IsValid(ygGM))
		{
			ygGM->PrintChatMessageString(this, InChatMessageString, isGuessNumber);
		}
	}

}

// 언제 인크리즈하냐 타이밍을 위해 컨트롤러로 옮김(추측이냐 아니냐만 판단하는 것이기 때문에 서버에서 안해도 무관)
bool Ayagu_PlayerController::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;
	do {
		if (InNumberString.Len() != 3)
		{
			break;
		}
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}
		bCanPlay = true;
	} while (false);
	return bCanPlay;
}

void Ayagu_PlayerController::IncreaseGuessCount()
{
	Ayagu_PlayerState* ygPS = GetPlayerState<Ayagu_PlayerState>();
	if (IsValid(ygPS))
	{
		ygPS->CurrentGuessCount++;
	}
}

// 서버에서 인크리즈 요청시 사용
void Ayagu_PlayerController::ClientRPCIncreaseGuessCount_Implementation()
{
	this->IncreaseGuessCount();
}

void Ayagu_PlayerController::setTurnCount_Implementation(const FText& InNotiText)
{
	if (IsValid(NotiWidgetInstance))
	{
		NotiWidgetInstance->SetTurnSecond(InNotiText);
	}
}

void Ayagu_PlayerController::setTurnText_Implementation(const FText& InNotiText)
{
	if (IsValid(NotiWidgetInstance))
	{
		NotiWidgetInstance->SetTurnNoti(InNotiText);
	}
}

void Ayagu_PlayerController::resetGuessCount_Implementation()
{
	Ayagu_PlayerState* ygPS = GetPlayerState<Ayagu_PlayerState>();
	if (IsValid(ygPS))
	{
		ygPS->CurrentGuessCount = 0;
	}
}