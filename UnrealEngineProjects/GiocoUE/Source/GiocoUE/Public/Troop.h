// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Troop.generated.h"

UCLASS()
class GIOCOUE_API ATroop : public AActor
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

	int32 GetMovement();

	int32 GetAttackRange();

	int32 GetMinAttackDamage();

	int32 GetMaxAttackDamage();

	int32 GetHealth();

	FString GetAttackType();

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
