// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "UEG_GamemodeBase.h"

// Sets default values
ATroop::ATroop()
{
	PrimaryActorTick.bCanEverTick = true;

	bSelected = false;
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

void ATroop::SetAttackType(EAttackType NewAttackType)
{
	AttackType = NewAttackType;
}

void ATroop::SetSelected(bool Value)
{
	bSelected = Value;
}

void ATroop::SetCurrentPath(TArray<pair<int32, int32>> Path)
{
	CurrentPath = Path;
	PathIndex = CurrentPath.Num() - 2;
	StepsTaken = 0;
	IsMoving = true;
	CurrentTime = FPlatformTime::Seconds();
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

EAttackType ATroop::GetAttackType()
{
	return AttackType;
}

bool ATroop::IsSelected()
{
	return bSelected;
}

// Called when the game starts or when spawned
void ATroop::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATroop::Tick(float DeltaTime)
{
	if (IsMoving)
	{
		if (PathIndex < 0)
		{
			AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
			IsMoving = false;
			FVector FinalLocation = GamemodeBase->GetGameField()->GetTile(CurrentPath[0].first, CurrentPath[0].second)->GetActorLocation();
			FinalLocation.Z = GamemodeBase->TroopPlacingHeight;
			SetActorLocation(FinalLocation);
			FVector2D GridLocation = GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(GetActorLocation());
			for (int32 J = CurrentPath.Num() - 1; J >= 0; J--)
			{
				UE_LOG(LogTemp, Log, TEXT("%d %d"), CurrentPath[J].first, CurrentPath[J].second);
			}
			UE_LOG(LogTemp, Log, TEXT("Grid: %f %f"), GridLocation.X, GridLocation.Y);
			int32 XPosition = FMath::RoundToInt(GridLocation.X);
			int32 YPosition = FMath::RoundToInt(GridLocation.Y);
			UE_LOG(LogTemp, Log, TEXT("Pos: %d %d"), XPosition, YPosition);
			GamemodeBase->GetGameField()->GetTile(CurrentPath[CurrentPath.Num() - 1].first, CurrentPath[CurrentPath.Num() - 1].second)->SetTroop(nullptr);
			GamemodeBase->GetGameField()->GetTile(CurrentPath[CurrentPath.Num() - 1].first, CurrentPath[CurrentPath.Num() - 1].second)->SetTileStatus(ETileStatus::FREE);
			GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTroop(this);
			GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTileStatus(ETileStatus::OCCUPIED);
			GamemodeBase->GetGameField()->EvaluatePossibleMoves(this);
			GamemodeBase->GetGameField()->FilterAttackTiles();
			GamemodeBase->GetGameField()->SetTilesMaterial();
		}
		else if (CurrentPath.Num() != 0)
		{
			AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
			FVector OldLocation = GamemodeBase->GetGameField()->GetTile(CurrentPath[PathIndex + 1].first, CurrentPath[PathIndex + 1].second)->GetActorLocation();
			OldLocation.Z = GamemodeBase->TroopPlacingHeight;
			FVector NewLocation = GamemodeBase->GetGameField()->GetTile(CurrentPath[PathIndex].first, CurrentPath[PathIndex].second)->GetActorLocation();
			NewLocation.Z = GamemodeBase->TroopPlacingHeight;
			FVector StepDistance = (NewLocation - OldLocation) / GamemodeBase->StepsForTile;
			if (FPlatformTime::Seconds() - CurrentTime >= GamemodeBase->StepTime)
			{
				SetActorLocation(GetActorLocation() + StepDistance);
				CurrentTime = FPlatformTime::Seconds();
				StepsTaken++;
				if (StepsTaken == GamemodeBase->StepsForTile)
				{
					StepsTaken = 0;
					PathIndex--;
				}
			}
		}
	}
}

