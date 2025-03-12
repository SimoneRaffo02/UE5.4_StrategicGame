// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerInterface.h"
#include "GameField.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UEG_GamemodeBase.generated.h"

/**
 *
 */
UCLASS()
class UE_STRATEGICGAME_API AUEG_GamemodeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUEG_GamemodeBase();

	TArray<IPlayerInterface*> Players;

	int32 CurrentPlayer;

	int32 ArchersNumber;

	int32 KnightsNumber;

	bool bArcherPlacingTurn = true;

	bool bKnightPlacingTurn = false;

	bool IsGameStarted = false;

	bool IsGameOver = false;

protected:

	UPROPERTY(EditAnywhere)
	int32 FieldSize;

	UPROPERTY(VisibleAnywhere)
	AGameField* GField;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGameField> GameFieldClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATroop> HumanArcherClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATroop> HumanKnightClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATroop> IAArcherClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATroop> IAKnightClass;

public:
	virtual void BeginPlay() override;

	void ChoosePlayerAndStartGame();

	void PlaceTroop(const int32 PlayerNumber, FVector& SpawnLocation);

	int32 GetNextPlayer(int32 Player);

	void TurnNextPlayer();
};
