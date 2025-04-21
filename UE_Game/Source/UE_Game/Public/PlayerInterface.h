// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UEG_GameInstance.h"
#include "Troop.h"
#include "Tile.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class UE_GAME_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	IPlayerInterface();

	int32 PlayerNumber;

	int32 PlacedArchers = 0;

	int32 PlacedKnights = 0;

	TArray<ATroop*> Troops;

	int32 GetPlacedArchers();

	int32 GetPlacedKnights();

	TArray<ATroop*>& GetTroops();

	void UpdatePlacedArchers();

	void UpdatePlacedKnights();

	bool EveryTroopMadeAction();

	bool EveryTroopAttacked();

	void IncreaseTroopActionArrayLength();

	void ResetTroopActionArray();

	virtual void OnTurn() {};

	virtual void OnWin() {};

	virtual void OnLose() {};

	virtual void Attack(ATroop& PlayerTroop, ATroop& EnemyTroop) {};

	virtual void Move(ATroop* Troop, ATile* Tile) {};

	virtual void EndTurn() {};

	UFUNCTION()
	virtual void ResetPlayer();

protected:
	TArray<bool> bTroopMadeAction;
};
