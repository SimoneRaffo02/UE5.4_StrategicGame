// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "PlayerInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HumanPlayer.generated.h"

UCLASS()
class UE_GAME_API AHumanPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHumanPlayer();

	virtual void OnTurn() override;

	virtual void OnWin() override;

	virtual void OnLose() override;

	virtual void Attack(ATroop& PlayerTroop, ATroop& EnemyTroop) override;

	virtual void Move(ATroop* Troop, ATile* Tile) override; 
	
	virtual void EndTurn() override;

	UFUNCTION()
	virtual void ResetPlayer() override;

	void SetMoved(bool Value);

	bool CanMove();

	ATroop* GetSelectedTroop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsMyTurn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ATroop* SelectedTroop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Moved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUEG_GameInstance* GameInstance;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnClick();
};
