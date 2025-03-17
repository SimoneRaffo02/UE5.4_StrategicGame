// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Troop.h"
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
	int32 PlayerNumber;

	int32 PlacedArchers = 0;

	int32 PlacedKnights = 0;

	TArray<ATroop*> Troops;

	int32 GetPlacedArchers();

	int32 GetPlacedKnights();

	TArray<ATroop*>& GetTroops();

	void UpdatePlacedArchers();

	void UpdatePlacedKnights();

	void ResetPlacedTroops();

	virtual void OnTurn() {};
};
