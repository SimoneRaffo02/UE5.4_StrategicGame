// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Troop.generated.h"

using namespace std;

class UArcherReachableTileDetails;

UENUM()
enum class EAttackType : uint8
{
	RANGED	UMETA(DisplayName = "RANGED"),
	MELEE	UMETA(DisplayName = "MELEE")
};

UCLASS()
class UE_GAME_API ATroop : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATroop();

	void SetOwnerPlayer(int32 Player);

	void SetAsArcher(int32 Player);

	void SetAsKnight(int32 Player);

	void SetMovement(int32 NewMovement);

	void SetAttackRange(int32 NewAttackRange);

	void SetMinAttackDamage(int32 NewMinAttackDamage);

	void SetMaxAttackDamage(int32 NewMaxAttackDamage);

	void SetMinCounterAttackDamage(int32 NewMinCounterAttackDamage);

	void SetMaxCounterAttackDamage(int32 NewMaxCounterAttackDamage);

	void SetHealth(int32 NewHealth);

	void SetAttackType(EAttackType NewAttackType);

	void SetSelected(bool Value);

	void SetTroopIndex(int32 NewTroopIndex);

	void SetCurrentPath(TArray<pair<int32, int32>> Path);

	int32 GetMovement();

	int32 GetAttackRange();

	int32 GetMinAttackDamage();

	int32 GetMaxAttackDamage();

	int32 GetMinCounterAttackDamage();

	int32 GetMaxCounterAttackDamage();

	int32 GetHealth();

	EAttackType GetAttackType();

	bool IsSelected();

	int32 GetTroopIndex();

	bool CanMove();

	void SetMoved(bool Value);

	bool GetMoved();

	void SetAttacked(bool Value);

	bool GetAttacked();

	bool CanAttack();

	int32 GetOwnerPlayer();

	int32 Attack(ATroop& EnemyTroop);

	void HideAndDisableCollision(); 

	void SetMoveTileDetails(UArcherReachableTileDetails* NewMoveTileDetails);
	
	UFUNCTION()
	void SelfDestroy();

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
	int32 MinCounterAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxCounterAttackDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAttackType AttackType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Moved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Attacked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TroopIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArcherReachableTileDetails* MoveTileDetails;

	TArray<std::pair<int32, int32>> CurrentPath;

	int32 PathIndex;

	int32 StepsTaken;

	double CurrentTime;

	int32 OwnerPlayer;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static const int32 ARCHER_MAX_HEALTH = 20;

	static const int32 KNIGHT_MAX_HEALTH = 40;
};
