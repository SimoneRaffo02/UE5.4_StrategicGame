// Fill out your copyright notice in the Description page of Project Settings.


#include "UEG_GameInstance.h"
#include "Troop.h"

UUEG_GameInstance::UUEG_GameInstance()
{
	PlayerScore = 0;
	AIScore = 0;
	Message = FString(TEXT("Sorteggio"));
}

void UUEG_GameInstance::SetMessage(FString NewMessage)
{
	Message = NewMessage;

	OnMessageChange.Broadcast();
}

void UUEG_GameInstance::AddMoveToHistory(int32 Player, EAttackType AttackType, FString StartTile, FString EndTile)
{
	FString PlayerString = TEXT("");
	FString TroopString = TEXT("");

	if (Player == 0)
	{
		PlayerString = TEXT("HP");
	}
	else
	{
		PlayerString = TEXT("AI");
	}

	if (AttackType == EAttackType::RANGED)
	{
		TroopString = TEXT("S");
	}
	else
	{
		TroopString = TEXT("B");
	}

	LastAction = FString::Printf(TEXT("%s : %s %s -> %s\n"), *PlayerString.Replace(TEXT("\n"), TEXT("")), *TroopString.Replace(TEXT("\n"), TEXT("")), *StartTile.Replace(TEXT("\n"), TEXT("")), *EndTile.Replace(TEXT("\n"), TEXT("")));

	OnAction.Broadcast();
}

void UUEG_GameInstance::AddAttackToHistory(int32 Player, EAttackType AttackType, FString AttackTile, int32 Damage)
{
	FString PlayerString = TEXT("");
	FString TroopString = TEXT("");
	FString DamageString = FString::FromInt(Damage);

	if (Player == 0)
	{
		PlayerString = TEXT("HP");
	}
	else
	{
		PlayerString = TEXT("AI");
	}

	if (AttackType == EAttackType::RANGED)
	{
		TroopString = TEXT("S");
	}
	else
	{
		TroopString = TEXT("B");
	}

	LastAction = FString::Printf(TEXT("%s : %s %s %s\n"), *PlayerString.Replace(TEXT("\n"), TEXT("")), *TroopString.Replace(TEXT("\n"), TEXT("")), *AttackTile.Replace(TEXT("\n"), TEXT("")), *DamageString.Replace(TEXT("\n"), TEXT("")));

	OnAction.Broadcast();
}

void UUEG_GameInstance::SetTurn()
{
	OnTurnChange.Broadcast();
}

void UUEG_GameInstance::IncreasePlayerScore()
{
	PlayerScore++;
	OnPointsChange.Broadcast();
}

void UUEG_GameInstance::IncreaseAIScore()
{
	AIScore++;
	OnPointsChange.Broadcast();
}

int32 UUEG_GameInstance::GetPlayerScore()
{
	return PlayerScore;
}

int32 UUEG_GameInstance::GetAIScore()
{
	return AIScore;
}

FString UUEG_GameInstance::GetMessage()
{
	return Message;
}

FString UUEG_GameInstance::GetLastAction()
{
	return LastAction;
}
