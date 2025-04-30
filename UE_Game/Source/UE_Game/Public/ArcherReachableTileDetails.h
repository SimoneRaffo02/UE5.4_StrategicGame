// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TroopReachableTileDetails.h"
#include "ArcherReachableTileDetails.generated.h"

/**
 * 
 */
UCLASS()
class UE_GAME_API UArcherReachableTileDetails : public UObject, public ITroopReachableTileDetails
{
	GENERATED_BODY()
public:
	UArcherReachableTileDetails();
	~UArcherReachableTileDetails();

	virtual void AddTile(ATile& NewTile, ATroop& CurrentTroop, AUEG_GamemodeBase& GamemodeBase) override;

	virtual void SearchAttackableEnemies(ATroop& AttackingTroop, AUEG_GamemodeBase& GamemodeBase) override;
};
