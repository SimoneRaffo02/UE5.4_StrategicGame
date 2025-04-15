// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "UEG_GamemodeBase.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TurnCounter)
	{
		TurnCounter->SetText(FText::FromString(TEXT("Turno: 0")));
	}

	if (EndTurnButton)
	{
		EndTurnButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnEndTurnButtonClick);
	}

	if (ResetGameButton)
	{
		ResetGameButton->OnClicked.AddDynamic(this, &UMyUserWidget::OnResetGameButtonClick);
	}

	GameInstance = Cast<UUEG_GameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OnMessageChange.AddDynamic(this, &UMyUserWidget::SetMessageText);
		GameInstance->OnTurnChange.AddDynamic(this, &UMyUserWidget::UpdateTurnCounter);
		GameInstance->OnAction.AddDynamic(this, &UMyUserWidget::AddToHistory);
		GameInstance->OnPointsChange.AddDynamic(this, &UMyUserWidget::UpdatePoints);
	}

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	if (GamemodeBase)
	{
		GamemodeBase->OnReset.AddDynamic(this, &UMyUserWidget::ResetUI);
		GamemodeBase->OnTroopHealthChange.AddDynamic(this, &UMyUserWidget::UpdateTroopsHealth);
	}
}

void UMyUserWidget::OnEndTurnButtonClick()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	if (GamemodeBase->CurrentPlayer == 0)
	{
		GamemodeBase->GetGameField()->RefreshGameField();
		GamemodeBase->Players[0]->EndTurn();
		GamemodeBase->TurnNextPlayer();
	}
}

void UMyUserWidget::OnResetGameButtonClick()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	GamemodeBase->OnReset.Broadcast();
	GamemodeBase->ChoosePlayerAndStartGame();
}

void UMyUserWidget::SetMessageText()
{
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(GameInstance->GetMessage()));
	}
}

void UMyUserWidget::UpdateTurnCounter()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	if (TurnCounter)
	{
		TurnCounter->SetText(FText::FromString(FString::Printf(TEXT("Turno: %d"), GamemodeBase->CurrentTurn)));
	}
}

void UMyUserWidget::AddToHistory()
{
	if (ActionsHistory)
	{
		FString ActionsText = ActionsHistory->GetText().ToString() + GameInstance->GetLastAction();
		ActionsHistory->SetText(FText::FromString(ActionsText));
		if (ActionsHistoryScrollBox)
		{
			ActionsHistoryScrollBox->ScrollToEnd();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No scroll panel"));
		}
	}
}

void UMyUserWidget::UpdateTroopsHealth()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	for (int32 PlayerIndex = 0; PlayerIndex < GamemodeBase->Players.Num(); PlayerIndex++)
	{
		for (ATroop* Troop : GamemodeBase->Players[PlayerIndex]->GetTroops())
		{
			if (Troop->GetAttackType() == EAttackType::RANGED)
			{
				if (PlayerIndex == 0)
				{
					if (HumanArcherHealthText)
					{
						HumanArcherHealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Troop->GetHealth(), ATroop::ARCHER_MAX_HEALTH)));
					}
				}
				else
				{
					if (AIArcherHealthText)
					{
						AIArcherHealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Troop->GetHealth(), ATroop::ARCHER_MAX_HEALTH)));
					}
				}
			}
			else
			{
				if (PlayerIndex == 0)
				{
					if (HumanKnightHealthText)
					{
						HumanKnightHealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Troop->GetHealth(), ATroop::KNIGHT_MAX_HEALTH)));
					}
				}
				else
				{
					if (AIKnightHealthText)
					{
						AIKnightHealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Troop->GetHealth(), ATroop::KNIGHT_MAX_HEALTH)));
					}
				}
			}
		}
	}
}

void UMyUserWidget::UpdatePoints()
{
	if (HPPointsText)
	{
		HPPointsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameInstance->GetPlayerScore())));
	}
	if (AIPointsText)
	{
		AIPointsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameInstance->GetAIScore())));
	}
}

void UMyUserWidget::ResetUI()
{
	if (TurnCounter)
	{
		TurnCounter->SetText(FText::FromString(TEXT("Turno: 0")));
	}
	if (ActionsHistory)
	{
		ActionsHistory->SetText(FText::FromString(TEXT("")));
	}
	if (HumanArcherHealthText)
	{
		HumanArcherHealthText->SetText(FText::FromString(TEXT("0 / 0")));
	}
	if (AIArcherHealthText)
	{
		AIArcherHealthText->SetText(FText::FromString(TEXT("0 / 0")));
	}
	if (HumanKnightHealthText)
	{
		HumanKnightHealthText->SetText(FText::FromString(TEXT("0 / 0")));
	}
	if (AIKnightHealthText)
	{
		AIKnightHealthText->SetText(FText::FromString(TEXT("0 / 0")));
	}
}
