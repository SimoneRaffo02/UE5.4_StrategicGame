// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"

// Sets default values
ATroop::ATroop()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATroop::SetMovement(int32 NewMovement)
{
	Movement = NewMovement;
}

void ATroop::SetAttackRange(int32 NewAttackRange)
{
	AttackRange = NewAttackRange;
}

void ATroop::SetMinAttackDamage(int32 NewMinAttackDamage)
{
	MinAttackDamage = NewMinAttackDamage;
}

void ATroop::SetMaxAttackDamage(int32 NewMaxAttackDamage)
{
	MaxAttackDamage = NewMaxAttackDamage;
}

void ATroop::SetHealth(int32 NewHealth)
{
	Health = NewHealth;
}

void ATroop::SetAttackType(FString NewAttackType)
{
	AttackType = NewAttackType;
}

int32 ATroop::GetMovement()
{
	return Movement;
}

int32 ATroop::GetAttackRange()
{
	return AttackRange;
}

int32 ATroop::GetMinAttackDamage()
{
	return MinAttackDamage;
}

int32 ATroop::GetMaxAttackDamage()
{
	return MaxAttackDamage;
}

int32 ATroop::GetHealth()
{
	return Health;
}

FString ATroop::GetAttackType()
{
	return AttackType;
}

// Called when the game starts or when spawned
void ATroop::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATroop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

