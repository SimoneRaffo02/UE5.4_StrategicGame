// Fill out your copyright notice in the Description page of Project Settings.

#include "RandomPlayer.h"
#include "UEG_GamemodeBase.h"
#include "GameField.h"
#include <utility>
#include <Kismet/GameplayStatics.h>

using namespace std;

// Sets default values
ARandomPlayer::ARandomPlayer() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentTroop = 0;
}

bool ARandomPlayer::TroopsCanMove()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	for (ATroop* Troop : GamemodeBase->Players[1]->GetTroops())
	{
		if (Troop->GetHealth() > 0 && Troop->CanMove())
		{
			return true;
		}
	}
	return false;
}

bool ARandomPlayer::TroopsCanAttack()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	for (ATroop* Troop : GamemodeBase->Players[1]->GetTroops())
	{
		if (Troop->GetHealth() > 0 && Troop->CanAttack())
		{
			return true;
		}
	}
	return false;
}

void ARandomPlayer::Attack(ATroop& PlayerTroop, ATroop& EnemyTroop)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	int32 Damage = PlayerTroop.Attack(EnemyTroop);
	GameInstance->AddAttackToHistory(1, PlayerTroop.GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTileByRelativeLocation(EnemyTroop.GetActorLocation())), Damage);
	GamemodeBase->GetGameField()->RefreshGameField();
	GamemodeBase->OnTroopHealthChange.Broadcast();
}

void ARandomPlayer::Move(ATroop* Troop, ATile* Tile)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	TArray<pair<int32, int32>> Path;
	pair<int32, int32> Src = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).Y);
	pair<int32, int32> Dest = make_pair(Tile->GetGridLocation().X, Tile->GetGridLocation().Y);
	GamemodeBase->GetGameField()->AStar(Src, Dest, Path);
	GamemodeBase->GetGameField()->RefreshGameField();
	GamemodeBase->GetGameField()->DrawPath(Path);
	Troop->SetCurrentPath(Path); GameInstance->AddMoveToHistory(1, Troop->GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Src.first, Src.second)), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Dest.first, Dest.second)));
}

void ARandomPlayer::EndTurn()
{
}

void ARandomPlayer::Action()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
			if (GamemodeBase->bArcherPlacingTurn || GamemodeBase->bKnightPlacingTurn)
			{
				int32 XPosition;
				int32 YPosition;
				bool bFreeTile = false;
				do {
					XPosition = FMath::RandRange(0, GamemodeBase->GetFieldSize() - 1);
					YPosition = FMath::RandRange(0, GamemodeBase->GetFieldSize() - 1);
					if (GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->GetTileStatus() == ETileStatus::FREE)
					{
						bFreeTile = true;
					}
				} while (!bFreeTile);
				FVector SpawnLocation = GamemodeBase->GetGameField()->GetRelativeLocationByXY(XPosition, YPosition);
				GamemodeBase->PlaceTroop(PlayerNumber, SpawnLocation);
				GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTileStatus(ETileStatus::OCCUPIED);
			}
			else if (GamemodeBase->IsGameStarted)
			{
				GamemodeBase->GetGameField()->RefreshGameField();

				while (Troops[CurrentTroop]->GetHealth() == 0)
				{
					NextTroop();
				}

				//Truppa che si deve muovere
				ATroop* SelectedTroop = Troops[CurrentTroop];

				// Se DoesMove è true si muove prima di attaccare, altrimenti attacca
				int32 DoesMove;
				if (SelectedTroop->CanMove() && SelectedTroop->CanAttack())
				{
					//Esiste almeno una truppa che può muoversi ed esiste almeno una truppa che può attaccare
					DoesMove = FMath::RandBool();
				}
				else if (SelectedTroop->CanMove())
				{
					//Esiste almeno una truppa che può muoversi anche se non esistono truppe che possono attaccare
					DoesMove = true;
				}
				else if (SelectedTroop->CanAttack())
				{
					//Esiste almeno una truppa che può attaccare anche se non esistono truppe che possono muoversi
					DoesMove = false;
				}

				if (DoesMove)
				{
					//Array con tutte le caselle su cui ci si può muovere
					TArray<ATile*> AvailableMoveTiles;

					GamemodeBase->GetGameField()->EvaluatePossibleMoves(SelectedTroop);

					//Prendiamo in considerazione tutte le caselle su cui ci si può muovere
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

					//Scelgo una casella randomica su cui muoversi
					ATile* SelectedTile = AvailableMoveTiles[FMath::RandRange(0, AvailableMoveTiles.Num() - 1)];

					Move(SelectedTroop, SelectedTile);
				}

				//Se in questo turno non è stata mossa nessuna truppa
				else
				{
					//Array con tutte le caselle che si possono attaccare
					TArray<ATile*> AvailableAttackTiles;

					GamemodeBase->GetGameField()->EvaluatePossibleMoves(SelectedTroop);

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

					Attack(*SelectedTroop, *EnemyTroop);

					if (!GamemodeBase->IsWinCondition())
					{
						do
						{
							NextTroop();
						} while (CurrentTroop != 0 && Troops[CurrentTroop]->GetHealth() == 0);

						if (CurrentTroop != 0)
						{
							Action();
						}
						else
						{
							GamemodeBase->TurnNextPlayer();
						}
					}
				}
			}
		}, 1, false);
}

int32 ARandomPlayer::NextTroop()
{
	CurrentTroop++;
	CurrentTroop %= Troops.Num();
	return CurrentTroop;
}

void ARandomPlayer::ResetPlayer()
{
	IPlayerInterface::ResetPlayer();

	CurrentTroop = 0;
}

void ARandomPlayer::OnTurn()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	if (GameInstance)
	{
		if (GamemodeBase->bArcherPlacingTurn)
		{
			GameInstance->SetMessage(TEXT("Piazzamento Arciere della AI"));
		}
		else if (GamemodeBase->bKnightPlacingTurn)
		{
			GameInstance->SetMessage(TEXT("Piazzamento Cavaliere della AI"));
		}
		else if (GamemodeBase->IsGameStarted)
		{
			GameInstance->SetMessage(TEXT("Turno AI"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Random Game Instance non creata"));
	}
	Action();
}

void ARandomPlayer::OnWin()
{
	if (GameInstance)
	{
		GameInstance->SetMessage(TEXT("Vince l'AI!"));
		GameInstance->IncreaseAIScore();
	}
}

void ARandomPlayer::OnLose()
{
}

// Called when the game starts or when spawned
void ARandomPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UUEG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	if (GamemodeBase)
	{
		GamemodeBase->OnReset.AddDynamic(this, &ARandomPlayer::ResetPlayer);
	}
}

// Called every frame
void ARandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

