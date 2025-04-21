// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "UEG_GamemodeBase.h"

// Sets default values
ATroop::ATroop()
{
	PrimaryActorTick.bCanEverTick = true;

	Moved = false;
	Attacked = false;
	bSelected = false;
}

void ATroop::SetAsArcher()
{
	SetMovement(3);
	SetAttackType(EAttackType::RANGED);
	SetAttackRange(10);
	SetMinAttackDamage(4);
	SetMaxAttackDamage(8);
	SetMinCounterAttackDamage(1);
	SetMaxCounterAttackDamage(3);
	SetHealth(ARCHER_MAX_HEALTH);
}

void ATroop::SetAsKnight()
{
	SetMovement(6);
	SetAttackType(EAttackType::MELEE);
	SetAttackRange(1);
	SetMinAttackDamage(1);
	SetMaxAttackDamage(6);
	SetMinCounterAttackDamage(1);
	SetMaxCounterAttackDamage(3);
	SetHealth(KNIGHT_MAX_HEALTH);
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

void ATroop::SetMinCounterAttackDamage(int32 NewMinCounterAttackDamage)
{
	MinCounterAttackDamage = NewMinCounterAttackDamage;
}

void ATroop::SetMaxCounterAttackDamage(int32 NewMaxCounterAttackDamage)
{
	MaxCounterAttackDamage = NewMaxCounterAttackDamage;
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

void ATroop::SetTroopIndex(int32 NewTroopIndex)
{
	TroopIndex = NewTroopIndex;
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

int32 ATroop::GetMinCounterAttackDamage()
{
	return MinCounterAttackDamage;
}

int32 ATroop::GetMaxCounterAttackDamage()
{
	return MaxCounterAttackDamage;
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

int32 ATroop::GetTroopIndex()
{
	return TroopIndex;
}

bool ATroop::CanMove()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	GamemodeBase->GetGameField()->EvaluatePossibleMoves(this);

	TArray<ATile*> AvailableMoveTiles;

	for (int32 Col = 0; Col < GamemodeBase->GetGameField()->GetFieldSize(); Col++)
	{
		for (int32 Row = 0; Row < GamemodeBase->GetGameField()->GetFieldSize(); Row++)
		{
			if (GamemodeBase->GetGameField()->GetTile(Col, Row)->GetMoveType() == EMoveType::MOVE)
			{
				AvailableMoveTiles.Add(GamemodeBase->GetGameField()->GetTile(Col, Row));
			}
		}
	}

	GamemodeBase->GetGameField()->ResetTilesMoveType();

	return AvailableMoveTiles.Num() > 0;
}

void ATroop::SetMoved(bool Value)
{
	Moved = Value;
}

bool ATroop::GetMoved()
{
	return Moved;
}

void ATroop::SetAttacked(bool Value)
{
	Attacked = Value;
}

bool ATroop::GetAttacked()
{
	return Attacked;
}

bool ATroop::CanAttack()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	GamemodeBase->GetGameField()->EvaluatePossibleMoves(this);

	TArray<ATile*> AvailableAttackTiles;

	for (int32 Col = 0; Col < GamemodeBase->GetGameField()->GetFieldSize(); Col++)
	{
		for (int32 Row = 0; Row < GamemodeBase->GetGameField()->GetFieldSize(); Row++)
		{
			if (GamemodeBase->GetGameField()->GetTile(Col, Row)->GetMoveType() == EMoveType::ATTACK)
			{
				AvailableAttackTiles.Add(GamemodeBase->GetGameField()->GetTile(Col, Row));
			}
		}
	}

	GamemodeBase->GetGameField()->ResetTilesMoveType();

	return AvailableAttackTiles.Num() > 0;
}

int32 ATroop::Attack(ATroop& EnemyTroop)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	int32 Damage = FMath::RandRange(MinAttackDamage, MaxAttackDamage);
	EnemyTroop.SetHealth(EnemyTroop.GetHealth() - Damage);
	if (EnemyTroop.GetHealth() <= 0)
	{
		EnemyTroop.SetHealth(0);
		GamemodeBase->GetGameField()->GetTileByRelativeLocation(EnemyTroop.GetActorLocation())->SetTroop(nullptr);
		GamemodeBase->GetGameField()->GetTileByRelativeLocation(EnemyTroop.GetActorLocation())->SetTileStatus(ETileStatus::FREE);
		EnemyTroop.HideAndDisableCollision();
	}
	FVector2D TroopGridLocation = GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(GetActorLocation());
	int32 TroopXPosition = FMath::RoundToInt(TroopGridLocation.X);
	int32 TroopYPosition = FMath::RoundToInt(TroopGridLocation.Y);
	FVector2D EnemyGridLocation = GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(EnemyTroop.GetActorLocation());
	int32 EnemyXPosition = FMath::RoundToInt(EnemyGridLocation.X);
	int32 EnemyYPosition = FMath::RoundToInt(EnemyGridLocation.Y);
	pair<int32, int32> EnemyCoordinates = make_pair(EnemyXPosition, EnemyYPosition);
	int32 EnemyDistance = GamemodeBase->GetGameField()->GetManhattanDistance(TroopXPosition, TroopYPosition, EnemyCoordinates);
	if (AttackType == EAttackType::RANGED && EnemyDistance <= EnemyTroop.GetAttackRange())
	{
		int32 CounterAttackDamage = FMath::RandRange(EnemyTroop.GetMinCounterAttackDamage(), EnemyTroop.GetMaxCounterAttackDamage());
		Health -= CounterAttackDamage;
		if (Health <= 0)
		{
			Health = 0;
			GamemodeBase->GetGameField()->GetTileByRelativeLocation(GetActorLocation())->SetTroop(nullptr);
			GamemodeBase->GetGameField()->GetTileByRelativeLocation(GetActorLocation())->SetTileStatus(ETileStatus::FREE);
			HideAndDisableCollision();
		}
	}
	return Damage;
}

void ATroop::HideAndDisableCollision()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ATroop::SelfDestroy()
{
	Destroy();
}

// Called when the game starts or when spawned
void ATroop::BeginPlay()
{
	Super::BeginPlay();

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	GamemodeBase->OnReset.AddDynamic(this, &ATroop::SelfDestroy);
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
			int32 XPosition = FMath::RoundToInt(GridLocation.X);
			int32 YPosition = FMath::RoundToInt(GridLocation.Y);
			GamemodeBase->GetGameField()->GetTile(CurrentPath[CurrentPath.Num() - 1].first, CurrentPath[CurrentPath.Num() - 1].second)->SetTroop(nullptr);
			GamemodeBase->GetGameField()->GetTile(CurrentPath[CurrentPath.Num() - 1].first, CurrentPath[CurrentPath.Num() - 1].second)->SetTileStatus(ETileStatus::FREE);
			GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTroop(this);
			GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTileStatus(ETileStatus::OCCUPIED);
			GamemodeBase->GetGameField()->EvaluatePossibleMoves(this);
			if (GamemodeBase->CurrentPlayer == 0)
			{
				GamemodeBase->GetGameField()->FilterAttackTiles();
				GamemodeBase->GetGameField()->SetTilesMaterial();
			}
			//Turno nemico
			else
			{
				GamemodeBase->GetGameField()->RefreshGameField();

				//Se la pedina mossa non può attaccare
				if (!CanAttack())
				{
					UE_LOG(LogTemp, Log, TEXT("Nemico non puo' attaccare: finisce il turno"));
					GamemodeBase->TurnNextPlayer();
					return;
				}
				else
				{
					//Array con tutte le caselle che si possono attaccare
					TArray<ATile*> AvailableAttackTiles;

					GamemodeBase->GetGameField()->EvaluatePossibleMoves(this);

					//Prendiamo in considerazione tutte le caselle che si possono attaccare
					for (int32 Col = 0; Col < GamemodeBase->GetGameField()->GetFieldSize(); Col++)
					{
						for (int32 Row = 0; Row < GamemodeBase->GetGameField()->GetFieldSize(); Row++)
						{
							if (GamemodeBase->GetGameField()->GetTile(Col, Row)->GetMoveType() == EMoveType::ATTACK)
							{
								AvailableAttackTiles.Add(GamemodeBase->GetGameField()->GetTile(Col, Row));
							}
						}
					}

					//Scelgo una casella randomica di cui attaccare la truppa
					ATile* SelectedTile = AvailableAttackTiles[FMath::RandRange(0, AvailableAttackTiles.Num() - 1)];
					ATroop* EnemyTroop = SelectedTile->GetTroop();

					GamemodeBase->Players[1]->Attack(*this, *EnemyTroop);
				}
			}
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

