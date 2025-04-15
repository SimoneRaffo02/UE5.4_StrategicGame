// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInterface.h"
#include "GameFramework/Pawn.h"
#include "RandomPlayer.generated.h"

UCLASS()
class UE_GAME_API ARandomPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARandomPlayer();

	bool TroopsCanMove();

	bool TroopsCanAttack();

	virtual void OnTurn() override;

	virtual void OnWin() override;

	virtual void OnLose() override;

	virtual void Attack(ATroop& PlayerTroop, ATroop& EnemyTroop) override;

	virtual void Move(ATroop* Troop, ATile* Tile) override;

	virtual void EndTurn() override;

	UFUNCTION()
	virtual void ResetPlayer() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUEG_GameInstance* GameInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
