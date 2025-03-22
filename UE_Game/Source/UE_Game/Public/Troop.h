// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Troop.generated.h"

UCLASS()
class UE_GAME_API ATroop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATroop();

	void SetMovement(int32 NewMovement);

	void SetAttackRange(int32 NewAttackRange);

	void SetMinAttackDamage(int32 NewMinAttackDamage);

	void SetMaxAttackDamage(int32 NewMaxAttackDamage);

	void SetHealth(int32 NewHealth);

	void SetAttackType(FString NewAttackType);

	void SetSelected(bool Value);

	int32 GetMovement();

	int32 GetAttackRange();

	int32 GetMinAttackDamage();

	int32 GetMaxAttackDamage();

	int32 GetHealth();

	FString GetAttackType();

	bool IsSelected();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 AttackRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MinAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString AttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSelected;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
