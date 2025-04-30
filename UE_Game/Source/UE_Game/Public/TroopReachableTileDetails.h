// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UEG_GamemodeBase.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TroopReachableTileDetails.generated.h"

class ATile;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTroopReachableTileDetails : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE_GAME_API ITroopReachableTileDetails
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	ATile* Tile;

	TArray<bool> IsInAttackRangeOfTroop;

	TArray<ATroop*> AttackableEnemies;

	TArray<int32> EnemyDistance;

	void Init(ATile& NewTile, ATroop& CurrentTroop, AUEG_GamemodeBase& GamemodeBase);

	int32 CountInAttackRangeOfEnemy();

	int32 GetMinEnemyDistance();

	void SortAttackableEnemies(ATroop& Troop);

	virtual void AddTile(ATile& NewTile, ATroop& CurrentTroop, AUEG_GamemodeBase& GamemodeBase) {};

	virtual void SearchAttackableEnemies(ATroop& AttackingTroop, AUEG_GamemodeBase& GamemodeBase) {};
};
