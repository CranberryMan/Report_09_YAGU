// Fill out your copyright notice in the Description page of Project Settings.


#include "game/yagu_gamemodebase.h"
#include "yagu_GameStateBase.h"
#include "Player/yagu_PlayerController.h"
#include "EngineUtils.h"
#include "Player/yagu_PlayerState.h"

void Ayagu_gamemodebase::BeginPlay()
{
	Super::BeginPlay();
	SecretNumberString = GenerateSecretNumber();
}

void Ayagu_gamemodebase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	Ayagu_PlayerController* ygPC = Cast<Ayagu_PlayerController>(NewPlayer);
	if (IsValid(ygPC))
	{
		AllPlayerControllers.Add(ygPC);
		Ayagu_PlayerState* ygPS = ygPC->GetPlayerState<Ayagu_PlayerState>();
		if (IsValid(ygPS))
		{
			ygPS->FS_Name = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		Ayagu_GameStateBase* ygGSB = GetGameState<Ayagu_GameStateBase>();
		if (IsValid(ygGSB))
		{
			ygGSB->MulticastRPCBroadcastLoginMessage(ygPS->FS_Name);
		}

		// ygPC->setNotiText(FText::FromString(TEXT("Connected to the game server.")));
	}

	int32 num = AllPlayerControllers.Num();
	UE_LOG(LogTemp, Warning, TEXT("Current Player Count: %d"), num);

	if (AllPlayerControllers.Num() >= 2 && !bIsGameStarted)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameStart"));
		bIsGameStarted = true;

		GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &Ayagu_gamemodebase::gameStart, 2.0f, false);
	}
}

void Ayagu_gamemodebase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	Ayagu_PlayerController* ygPC = Cast<Ayagu_PlayerController>(Exiting);
	if (IsValid(ygPC))
	{
		// 로그아웃한 유저는 제거
		if (AllPlayerControllers.Remove(ygPC) > 0)
		{
			int32 num = AllPlayerControllers.Num();
			UE_LOG(LogTemp, Warning, TEXT("Current Player Count: %d"), num);
		}
	}
}

void Ayagu_gamemodebase::PrintChatMessageString(Ayagu_PlayerController* InChattingPlayerController, const FString& InChatMessageString, bool IsGuessNumberString)
{
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	if (IsGuessNumberString)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);		

		for (TActorIterator<Ayagu_PlayerController> It(GetWorld()); It; ++It)
		{
			Ayagu_PlayerController* ygPC = *It;
			if (IsValid(ygPC))
			{
				/// 게임에 참가한 유저가 아니라면 Judge 없이 메시지 출력한다
				bool isGamer = false;
				for (auto& a : gamePlayerControllers)
				{
					if (a == ygPC)
					{
						isGamer = true;
						ygPC->turnOff = true;
						break;
					}
				}
				if (!isGamer)
				{
					ygPC->ClientRPCPrintChatMessageString(InChatMessageString);
					continue;
				}

				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				ygPC->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
	}
	else
	{
		for (TActorIterator<Ayagu_PlayerController> It(GetWorld()); It; ++It)
		{
			Ayagu_PlayerController* ygPC = *It;
			if (IsValid(ygPC))
			{
				
				ygPC->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

FString Ayagu_gamemodebase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	UE_LOG(LogTemp, Warning, TEXT("Secret Number: %s"), *Result);
	return Result;
}

FString Ayagu_gamemodebase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;
	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	return FString::Printf(TEXT("%dS, %dB"), StrikeCount, BallCount);
}

void Ayagu_gamemodebase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	/// 턴 관련 정보 초기화
	nowTurn = 0;
	TimerCount = 0;

	UE_LOG(LogTemp, Warning, TEXT("RESET"));
	for (const auto& ygPC : gamePlayerControllers)
	{
		ygPC->resetGuessCount();
		Ayagu_PlayerState* ygPS = ygPC->GetPlayerState<Ayagu_PlayerState>();
		ygPC->setTurnText(FText::FromString("Soon Game Start Please Wait..."));
		
		int32 num = AllPlayerControllers.Num();
		UE_LOG(LogTemp, Warning, TEXT("Current Player Count: %d"), num);

		if (IsValid(ygPS))
		{
			ygPS->isPlayer = false;
		}
	}
	
	bIsGameStarted = false;
	gamePlayerControllers.Empty();

	if (AllPlayerControllers.Num() >= 2 && !bIsGameStarted)
	{
		bIsGameStarted = true;

		GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &Ayagu_gamemodebase::gameStart, 5.0f, false);
	}
}

void Ayagu_gamemodebase::JudgeGame(Ayagu_PlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		Ayagu_PlayerState* ygPS = InChattingPlayerController->GetPlayerState<Ayagu_PlayerState>();
		for (const auto& ygPC : AllPlayerControllers)
		{
			if (IsValid(ygPS))
			{
				FString CombinedMessageString = ygPS->FS_Name + TEXT(" has won the game. after 5second, new game start");
				ygPC->setNotiText(FText::FromString(CombinedMessageString));
			}

		}
		ResetGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& ygPC : AllPlayerControllers)
		{
			Ayagu_PlayerState* ygPS = ygPC->GetPlayerState<Ayagu_PlayerState>();
			if (IsValid(ygPS))
			{
				if (ygPS->CurrentGuessCount < ygPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& ygPC : AllPlayerControllers)
			{
				ygPC->setNotiText(FText::FromString(TEXT("Draw...")));
			}
			ResetGame();
		}
	}
}

void Ayagu_gamemodebase::gameStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Inside"));
	for (const auto& ygPC : AllPlayerControllers)
	{
		ygPC->setNotiText(FText::FromString(TEXT("Game Start!")));
		gamePlayerControllers.Add(ygPC);
		
		// 턴 사용 초기화
		ygPC->turnOff = false;

		Ayagu_PlayerState* ygPS = ygPC->GetPlayerState<Ayagu_PlayerState>();
		if(IsValid(ygPS))
		{
			ygPS->isPlayer = true;
		}
	}
	turnStart();
}

void Ayagu_gamemodebase::turnStart()
{
	nowTurn++;
	TimerCount = 0;
	// 턴 초과로 드로우 엔드
	if (nowTurn > 3)
	{
		/// 일단 타이머 먼저 정지
		GetWorld()->GetTimerManager().ClearTimer(GameCountTimerHandle);

		for (const auto& ygPC : AllPlayerControllers)
		{
			ygPC->setNotiText(FText::FromString(TEXT("Turn Over Draw...")));
		}		
		ResetGame();
		return;
	}

	for (const auto& ygPC : gamePlayerControllers)
	{
		// 첫 턴에는 강제 증가시키지 않음
		// 턴 사용 안했는데 턴이 넘어갔을때 횟수 증가
		if (!ygPC->turnOff && nowTurn != 1)
		{
			ygPC->ClientRPCIncreaseGuessCount();
		}

		ygPC->turnOff = false;
		ygPC->setTurnText(FText::FromString(FString::Printf(TEXT("Turn %d"), nowTurn)));
		ygPC->setNotiText(FText::FromString(FString::Printf(TEXT("Please Input Your Answer"))));
	}
	GetWorld()->GetTimerManager().SetTimer(GameCountTimerHandle, this, &Ayagu_gamemodebase::turnCountTimer, 1.0f, true);
}

void Ayagu_gamemodebase::turnCountTimer()
{
	TimerCount++;
	UE_LOG(LogTemp, Warning, TEXT("Game Start Count: %d"), TimerCount);

	// 5초 도달했는지 확인용 (여유롭게 1초 더 줌 ㅎㅎ)
	if (TimerCount >= 6)
	{
		GetWorld()->GetTimerManager().ClearTimer(GameCountTimerHandle);

		for (const auto& ygPC : gamePlayerControllers)
		{
			ygPC->setTurnCount(FText::FromString(FString::Printf(TEXT("%d seconds left"), 6 - TimerCount)));
		}

		turnStart();
	}
	else
	{
		// 5회 미만일 때 매번 실행할 로직
		// 예: 카운트다운 숫자 감소 UI 업데이트
		for (const auto& ygPC : gamePlayerControllers)
		{
			ygPC->setTurnCount(FText::FromString(FString::Printf(TEXT("%d seconds left"), 6 - TimerCount)));
		}
	}
}

/*
	턴중에 두 번 이상 입력 못하게 만들기
	3턴 안에 누군가 게임을 끝내면 다같이 정지시키게 만들기
*/