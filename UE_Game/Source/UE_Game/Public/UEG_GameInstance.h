// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UEG_GameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMessageChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAction);


UCLASS()
class UE_GAME_API UUEG_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UUEG_GameInstance();
	
	UFUNCTION()
	void SetMessage(FString NewMessage);

	UFUNCTION()
	void AddMoveToHistory(int32 Player, EAttackType AttackType, FString StartTile, FString EndTile);

	UFUNCTION()
	void AddAttackToHistory(int32 Player, EAttackType AttackType, FString AttackTile, int32 Damage);

	void SetTurn();

	void IncreasePlayerScore();

	void IncreaseIAScore();

	FString GetMessage();

	FString GetLastAction();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMessageChange OnMessageChange;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTurnChange OnTurnChange;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAction OnAction;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Message;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString LastAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PlayerScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 IAScore;
};
