// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/yagu_Notification.h"
#include "Components/TextBlock.h"


void Uyagu_Notification::SetChatLog(const FText& NewMessage)
{
	if (IsValid(NotiList))
	{
		NotiList->SetText(NewMessage);
	}
}

void Uyagu_Notification::SetTurnNoti(const FText& NewMessage)
{
	if (IsValid(TurnNoti))
	{
		TurnNoti->SetText(NewMessage);
	}
}

void Uyagu_Notification::SetTurnSecond(const FText& NewMessage)
{
	if (IsValid(TurnSecond))
	{
		TurnSecond->SetText(NewMessage);
	}
}
