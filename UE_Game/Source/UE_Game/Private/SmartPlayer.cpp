// Fill out your copyright notice in the Description page of Project Settings.

#include "SmartPlayer.h"
#include "UEG_GamemodeBase.h"
#include "GameField.h"
#include <utility>
#include <Kismet/GameplayStatics.h>


// Sets default values
ASmartPlayer::ASmartPlayer() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentTroop = 0;
}

void ASmartPlayer::OnTurn()
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

void ASmartPlayer::OnWin()
{
	if (GameInstance)
	{
		GameInstance->SetMessage(TEXT("Vince l'AI!"));
		GameInstance->IncreaseAIScore();
	}
}

void ASmartPlayer::Attack(ATroop& PlayerTroop, ATroop& EnemyTroop)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	int32 Damage = PlayerTroop.Attack(EnemyTroop);
	GameInstance->AddAttackToHistory(1, PlayerTroop.GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTileByRelativeLocation(EnemyTroop.GetActorLocation())), Damage);
	GamemodeBase->GetGameField()->ResetTilesMoveType();
	GamemodeBase->OnTroopHealthChange.Broadcast();
}

void ASmartPlayer::Move(ATroop* Troop, ATile* Tile)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	TArray<pair<int32, int32>> Path;
	pair<int32, int32> Src = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(Troop->GetActorLocation()).Y);
	pair<int32, int32> Dest = make_pair(Tile->GetGridLocation().X, Tile->GetGridLocation().Y);
	GamemodeBase->GetGameField()->AStar(Src, Dest, Path);
	GamemodeBase->GetGameField()->RefreshGameField();
	GamemodeBase->GetGameField()->DrawPath(Path);
	Troop->SetCurrentPath(Path); 
	GameInstance->AddMoveToHistory(1, Troop->GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Src.first, Src.second)), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Dest.first, Dest.second)));
}

void ASmartPlayer::EndTurn()
{
}

int32 ASmartPlayer::NextTroop()
{
	CurrentTroop++;
	CurrentTroop %= Troops.Num();
	return CurrentTroop;
}

void ASmartPlayer::Action()
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

				//Array con tutte le caselle su cui ci si può muovere
				TArray<ATile*> AvailableMoveTiles;

				GamemodeBase->GetGameField()->EvaluatePossibleMoves(SelectedTroop);

				//Prendiamo in considerazione tutte le caselle su cui ci si può muovere (compresa quella su cui è la truppa)
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
				AvailableMoveTiles.Add(GamemodeBase->GetGameField()->GetTileByRelativeLocation(SelectedTroop->GetActorLocation()));

				//Ripulisco il campo
				GamemodeBase->GetGameField()->RefreshGameField();

				TArray<UArcherReachableTileDetails*> TilesDetails;

				//Memorizzo tutte le informazioni necessarie alle caselle in cui la truppa può muoversi
				for (ATile* Tile : AvailableMoveTiles)
				{
					UArcherReachableTileDetails* TileDetails = NewObject<UArcherReachableTileDetails>();
					TileDetails->Init(*Tile, *SelectedTroop, *GamemodeBase);
					TilesDetails.Add(TileDetails);
				}

				//Elimino tutte le informazioni relative alle caselle da cui la truppa non può attaccare
				for (int32 I = TilesDetails.Num() - 1; I >= 0; I--)
				{
					if (TilesDetails[I]->AttackableEnemies.Num() == 0)
					{
						TilesDetails.RemoveAt(I);
					}
				}

				//Se esiste almeno una casella in cui può attaccare dei nemici ci si sposta e attacca
				if (TilesDetails.Num() > 0)
				{
					//Riordiniamo le informazioni:
					SortTilesDetails(TilesDetails);

					//Memorizzo le informazioni della casella in cui si sposta la truppa (la prima, ossia la migliore secondo i criteri scelti)
					UArcherReachableTileDetails* MoveTileDetails = TilesDetails[0];
						
					//Se la casella è diversa da quella in cui è la truppa si sposta
					if (MoveTileDetails->Tile != GamemodeBase->GetGameField()->GetTileByRelativeLocation(SelectedTroop->GetActorLocation()))
					{
						GamemodeBase->GetGameField()->EvaluatePossibleMoves(SelectedTroop);
						SelectedTroop->SetMoveTileDetails(MoveTileDetails);
						Move(SelectedTroop, MoveTileDetails->Tile);
						GamemodeBase->GetGameField()->RefreshGameField();
					}

					//Altrimenti attacco
					else
					{
						//Se posso attaccare più di un nemico seleziono il nemico da attaccare in base ai criteri del riordinamento
						if (MoveTileDetails->AttackableEnemies.Num() > 1)
						{
							MoveTileDetails->SortAttackableEnemies(*SelectedTroop);
						}

						Attack(*SelectedTroop, *MoveTileDetails->AttackableEnemies[0]);

						GamemodeBase->GetGameField()->RefreshGameField();

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
				//Altrimenti si sposta verso il nemico più vicino
				else
				{
					ATroop* ClosestEnemy = GetClosestEnemy(*SelectedTroop, *GamemodeBase);

					TArray<pair<int32, int32>> Path;
					pair<int32, int32> Src = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).Y);
					pair<int32, int32> Dest = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(ClosestEnemy->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(ClosestEnemy->GetActorLocation()).Y);

					TMap<ATroop*, ATile*> TroopsPositions;
					for (int32 I = 0; I < GamemodeBase->Players.Num(); I++)
					{
						for (ATroop* Troop : GamemodeBase->Players[I]->GetTroops())
						{
							ATile* Tile = GamemodeBase->GetGameField()->GetTileByRelativeLocation(Troop->GetActorLocation());
							TroopsPositions.Add(Troop, Tile);
							Tile->SetTroop(nullptr);
							Tile->SetTileStatus(ETileStatus::FREE);
						}
					}

					GamemodeBase->GetGameField()->SetEveryTileMoveTile();

					GamemodeBase->GetGameField()->AStar(Src, Dest, Path);

					for (int32 I = 0; I < GamemodeBase->Players.Num(); I++)
					{
						for (ATroop* Troop : GamemodeBase->Players[I]->GetTroops())
						{
							if (Troop->GetHealth() > 0)
							{
								ATile* Tile = GamemodeBase->GetGameField()->GetTileByRelativeLocation(Troop->GetActorLocation());
								Tile->SetTroop(Troop);
								Tile->SetTileStatus(ETileStatus::OCCUPIED);
							}
						}
					}
					TroopsPositions.Empty();

					GamemodeBase->GetGameField()->RefreshGameField();
						
					for (int32 I = Path.Num() - 2 - SelectedTroop->GetMovement(); I >= 0; I--)
					{
						Path.RemoveAt(I);
					}

					for (int32 I = Path.Num() - 1; I >= 0; I--)
					{
						if (GamemodeBase->GetGameField()->GetTile(Path[I].first, Path[I].second)->GetTroop() != nullptr && GamemodeBase->GetGameField()->GetTile(Path[I].first, Path[I].second)->GetTroop() != SelectedTroop)
						{
							Path.RemoveAt(0, I + 1);
							break;
						}
					}

					if (Path.Num() > 0)
					{
						GamemodeBase->GetGameField()->DrawPath(Path);
						GameInstance->AddMoveToHistory(1, SelectedTroop->GetAttackType(), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Src.first, Src.second)), GamemodeBase->GetGameField()->GetTileName(GamemodeBase->GetGameField()->GetTile(Dest.first, Dest.second)));

						SelectedTroop->SetMoveTileDetails(nullptr);
						SelectedTroop->SetCurrentPath(Path);
					}
					else
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

//Riordinamento delle informazioni delle tile in base ai seguenti criteri utilizzando il bubble sort:
//- cella meno attaccata dai nemici
//- distanza minima maggiore dai nemici che posso attaccare
//- numero di nemici attaccabili
void ASmartPlayer::SortTilesDetails(TArray<UArcherReachableTileDetails*>& TilesDetails)
{
	for (int32 I = 0; I < TilesDetails.Num() - 1; I++)
	{
		for (int32 J = I + 1; J < TilesDetails.Num(); J++)
		{
			if (TilesDetails[J]->CountInAttackRangeOfEnemy() < TilesDetails[I]->CountInAttackRangeOfEnemy())
			{
				UArcherReachableTileDetails* tmpVar = TilesDetails[J];
				TilesDetails[J] = TilesDetails[I];
				TilesDetails[I] = tmpVar;
			}
			else if (TilesDetails[J]->CountInAttackRangeOfEnemy() == TilesDetails[I]->CountInAttackRangeOfEnemy())
			{
				if (TilesDetails[J]->GetMinEnemyDistance() > TilesDetails[I]->GetMinEnemyDistance())
				{
					UArcherReachableTileDetails* tmpVar = TilesDetails[J];
					TilesDetails[J] = TilesDetails[I];
					TilesDetails[I] = tmpVar;
				}
				else if (TilesDetails[J]->GetMinEnemyDistance() == TilesDetails[I]->GetMinEnemyDistance())
				{
					if (TilesDetails[J]->AttackableEnemies.Num() > TilesDetails[I]->AttackableEnemies.Num())
					{
						UArcherReachableTileDetails* tmpVar = TilesDetails[J];
						TilesDetails[J] = TilesDetails[I];
						TilesDetails[I] = tmpVar;
					}
				}
			}
		}
	}
}

ATroop* ASmartPlayer::GetClosestEnemy(ATroop& Troop, AUEG_GamemodeBase& GamemodeBase)
{
	pair<int32, int32> TroopCoordinates = make_pair(GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Troop.GetActorLocation()).X, GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Troop.GetActorLocation()).Y);
	int32 MinDistance = MAX_int32;
	ATroop* ClosestEnemy = nullptr;

	for (ATroop* Enemy : GamemodeBase.Players[0]->GetTroops())
	{
		int32 EnemyCol = GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Enemy->GetActorLocation()).X;
		int32 EnemyRow = GamemodeBase.GetGameField()->GetGridLocationByRelativeLocation(Enemy->GetActorLocation()).Y;
		if (GamemodeBase.GetGameField()->GetManhattanDistance(EnemyCol, EnemyRow, TroopCoordinates) < MinDistance)
		{
			MinDistance = GamemodeBase.GetGameField()->GetManhattanDistance(EnemyCol, EnemyRow, TroopCoordinates);
			ClosestEnemy = Enemy;
		}
	}

	return ClosestEnemy;
}

void ASmartPlayer::ResetPlayer()
{
	IPlayerInterface::ResetPlayer();

	CurrentTroop = 0;
}

// Called when the game starts or when spawned
void ASmartPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UUEG_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());

	if (GamemodeBase)
	{
		GamemodeBase->OnReset.AddDynamic(this, &ASmartPlayer::ResetPlayer);
	}
}

// Called every frame
void ASmartPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASmartPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

