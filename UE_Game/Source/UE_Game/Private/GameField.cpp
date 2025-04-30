#include "GameField.h"
#include "UEG_GamemodeBase.h"

AGameField::AGameField()
{
	PrimaryActorTick.bCanEverTick = false;

	TileSize = 100;
	FieldSize = 25;
}

void AGameField::GenerateField()
{
	for (int32 X = 0; X < FieldSize; X++)
	{
		for (int32 Y = 0; Y < FieldSize; Y++)
		{
			FVector Location = AGameField::GetRelativeLocationByXY(X, Y);
			ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);
			float TileScale = TileSize / 100.f;
			Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2f));
			Obj->SetGridLocation(X, Y);
			Field.Add(FVector2D(X, Y), Obj);
		}
	}
}

void AGameField::GenerateObstacles()
{
	float ObstacleTiles = 0.f;
	float Tiles = FieldSize * FieldSize;
	int32 InvalidPositioningTimes = 0;
	while (ObstacleTiles / Tiles < ObstaclesPercentage / 100 && InvalidPositioningTimes < 20)
	{
		int32 XPosition = FMath::RandRange(0, FieldSize - 1);
		int32 YPosition = FMath::RandRange(0, FieldSize - 1);
		if (Field[FVector2D(XPosition, YPosition)]->GetTileStatus() == ETileStatus::FREE)
		{
			if (!CheckUnreachableTiles(XPosition, YPosition))
			{
				Field[FVector2D(XPosition, YPosition)]->SetTileStatus(ETileStatus::OBSTACLE);
				ObstacleTiles++;
				InvalidPositioningTimes = 0;
			}
			else
			{
				InvalidPositioningTimes++;
			}
		}
	}
}

void AGameField::RefreshGameField()
{
	ResetTilesMoveType();
	SetTilesMaterial();
}

void AGameField::SetFieldSize(int32 NewFieldSize)
{
	FieldSize = NewFieldSize;
}

void AGameField::SetObstaclesPercentage(float NewObstaclesPercentage)
{
	ObstaclesPercentage = NewObstaclesPercentage;
}

void AGameField::SetTilesMaterial()
{
	for (int32 X = 0; X < FieldSize; X++)
	{
		for (int32 Y = 0; Y < FieldSize; Y++)
		{
			Field[FVector2D(X, Y)]->SetTilesMaterial();
		}
	}
}

void AGameField::SetEveryTileMoveTile()
{
	for (int32 X = 0; X < FieldSize; X++)
	{
		for (int32 Y = 0; Y < FieldSize; Y++)
		{
			if (Field[FVector2D(X, Y)]->GetTileStatus() == ETileStatus::FREE)
			{
				Field[FVector2D(X, Y)]->SetMoveType(EMoveType::MOVE);
			}
		}
	}
}

bool AGameField::CheckUnreachableTiles(int32 XPosition, int32 YPosition)
{
	//Creo una scacchiera basata su quella attuale che useremo per l'analisi del nuovo ostacolo
	TMap<FVector2D, TArray<bool>> AnalysisField;
	for (int32 IIndex = 0; IIndex < FieldSize; IIndex++)
	{
		for (int32 JIndex = 0; JIndex < FieldSize; JIndex++)
		{
			FVector2D Location = FVector2D(IIndex, JIndex);
			TArray<bool> FieldElement;
			FieldElement.SetNum(2);
			//L'elemento 0 risponde alla domanda "E' ostacolo?"
			FieldElement[0] = Field[Location]->GetTileStatus() == ETileStatus::OBSTACLE;
			//L'elemento 1 risponde alla domanda "E' raggiungibile?"
			FieldElement[1] = false;
			AnalysisField.Add(Location, FieldElement);
		}
	}
	//Aggiungo il nuovo ostacolo alla scacchiera di analisi
	AnalysisField[FVector2D(XPosition, YPosition)][0] = true;

	//Cerco una casella non ostacolo e mi salvo la sua posizione
	bool bFoundFreeTile = false;
	FVector2D FreeTileLocation;
	for (int32 IIndex = 0; IIndex < FieldSize && !bFoundFreeTile; IIndex++)
	{
		for (int32 JIndex = 0; JIndex < FieldSize && !bFoundFreeTile; JIndex++)
		{
			FVector2D Location = FVector2D(IIndex, JIndex);
			if (!AnalysisField[Location][0])
			{
				AnalysisField[FVector2D(IIndex, JIndex)][1] = true;
				bFoundFreeTile = true;
				FreeTileLocation = Location;
			}
		}
	}

	//Segno tutte le caselle raggiungibili dalla casella non ostacolo trovata precedentemente 
	UpdateAnalysisField(AnalysisField, FreeTileLocation);

	//Cerco se esiste qualche casella non ostacolo non raggiungibile dalla casella non ostacolo trovata precedentemente
	for (int32 IIndex = 0; IIndex < FieldSize; IIndex++)
	{
		for (int32 JIndex = 0; JIndex < FieldSize; JIndex++)
		{
			if (!AnalysisField[FVector2D(IIndex, JIndex)][0] && !AnalysisField[FVector2D(IIndex, JIndex)][1])
			{
				//C'è almeno una casella non raggiungibile
				return true;
			}
		}
	}

	//Non ci sono caselle irraggiungibili
	return false;
}

void AGameField::UpdateAnalysisField(TMap<FVector2D, TArray<bool>>& AnalysisField, FVector2D CurrentLocation)
{
	if (CurrentLocation.X - 1 >= 0 && !AnalysisField[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)][0] && !AnalysisField[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)][1])
	{
		AnalysisField[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)][1] = true;
		UpdateAnalysisField(AnalysisField, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y));
	}
	if (CurrentLocation.X + 1 < FieldSize && !AnalysisField[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)][0] && !AnalysisField[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)][1])
	{
		AnalysisField[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)][1] = true;
		UpdateAnalysisField(AnalysisField, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y));
	}
	if (CurrentLocation.Y - 1 >= 0 && !AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)][0] && !AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)][1])
	{
		AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)][1] = true;
		UpdateAnalysisField(AnalysisField, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1));
	}
	if (CurrentLocation.Y + 1 < FieldSize && !AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)][0] && !AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)][1])
	{
		AnalysisField[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)][1] = true;
		UpdateAnalysisField(AnalysisField, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1));
	}
}

void AGameField::EvaluatePossibleMoves(ATroop* Troop)
{
	FVector2D TroopLocation = GetGridLocationByRelativeLocation(Troop->GetActorLocation());
	int32 XPosition = FMath::RoundToInt(TroopLocation.X);
	int32 YPosition = FMath::RoundToInt(TroopLocation.Y);
	TroopLocation = FVector2D(XPosition, YPosition);
	SearchMovePaths(0, *Troop, TroopLocation);
	SearchAttackPaths(*Troop);
}

void AGameField::SearchMovePaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation)
{
	if (StepNumber < Troop.GetMovement())
	{
		if (Field.Find(CurrentLocation) && Field[CurrentLocation]->GetTileStatus() == ETileStatus::OCCUPIED && Field[CurrentLocation]->GetTroop() != &Troop)
		{
			return;
		}
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (CurrentLocation.X - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)))
		{
			ATile* LeftTile = Field[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)];
			if (LeftTile->GetTileStatus() == ETileStatus::FREE)
			{
				LeftTile->SetMoveType(EMoveType::MOVE);
				SearchMovePaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y));
			}
		}
		if (CurrentLocation.X + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)))
		{
			ATile* RightTile = Field[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)];
			if (RightTile->GetTileStatus() == ETileStatus::FREE)
			{
				RightTile->SetMoveType(EMoveType::MOVE);
				SearchMovePaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y));
			}
		}
		if (CurrentLocation.Y - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)))
		{
			ATile* UpperTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)];
			if (UpperTile->GetTileStatus() == ETileStatus::FREE)
			{
				UpperTile->SetMoveType(EMoveType::MOVE);
				SearchMovePaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1));
			}
		}
		if (CurrentLocation.Y + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)))
		{
			ATile* LowerTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)];
			if (LowerTile->GetTileStatus() == ETileStatus::FREE)
			{
				LowerTile->SetMoveType(EMoveType::MOVE);
				SearchMovePaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1));
			}
		}
	}
}

void AGameField::SearchAttackPaths(ATroop& Troop)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	AGameField* GameField = GamemodeBase->GetGameField();
	FVector2D TroopLocation = GameField->GetTileByRelativeLocation(Troop.GetActorLocation())->GetGridLocation();

	if (Troop.GetAttackType() == EAttackType::RANGED)
	{
		for (ATroop* EnemyTroop : GamemodeBase->Players[GamemodeBase->GetNextPlayer(Troop.GetOwnerPlayer())]->GetTroops())
		{
			if (EnemyTroop->GetHealth() > 0)
			{
				FVector2D EnemyLocation = GameField->GetTileByRelativeLocation(EnemyTroop->GetActorLocation())->GetGridLocation();

				int32 Distance = FMath::Abs(TroopLocation.X - EnemyLocation.X) + FMath::Abs(TroopLocation.Y - EnemyLocation.Y);
				if (Distance <= Troop.GetAttackRange())
				{
					GameField->GetTileByRelativeLocation(EnemyTroop->GetActorLocation())->SetMoveType(EMoveType::ATTACK);
				}
			}
		}
	}
	else if (Troop.GetAttackType() == EAttackType::MELEE)
	{

		for (ATroop* EnemyTroop : GamemodeBase->Players[GamemodeBase->GetNextPlayer(Troop.GetOwnerPlayer())]->GetTroops())
		{
			if (EnemyTroop->GetHealth() > 0)
			{
				FVector2D EnemyLocation = GameField->GetTileByRelativeLocation(EnemyTroop->GetActorLocation())->GetGridLocation();

				GameField->GetTile(EnemyLocation.X, EnemyLocation.Y)->SetTroop(nullptr);
				GameField->GetTile(EnemyLocation.X, EnemyLocation.Y)->SetTileStatus(ETileStatus::FREE);
				GameField->GetTile(EnemyLocation.X, EnemyLocation.Y)->SetMoveType(EMoveType::MOVE);

				TArray<pair<int32, int32>> Path;
				pair<int32, int32> Src = make_pair(TroopLocation.X, TroopLocation.Y);
				pair<int32, int32> Dest = make_pair(EnemyLocation.X, EnemyLocation.Y);
				GamemodeBase->GetGameField()->AStar(Src, Dest, Path);

				if (Path.Num() > 0)
				{
					Path.Pop();
				}

				GameField->GetTile(EnemyLocation.X, EnemyLocation.Y)->SetTroop(EnemyTroop);
				GameField->GetTile(EnemyLocation.X, EnemyLocation.Y)->SetTileStatus(ETileStatus::OCCUPIED);

				if (Path.Num() > 0 && Path.Num() <= Troop.GetAttackRange())
				{
					GameField->GetTileByRelativeLocation(EnemyTroop->GetActorLocation())->SetMoveType(EMoveType::ATTACK);
				}
				else
				{
					GameField->GetTileByRelativeLocation(EnemyTroop->GetActorLocation())->SetMoveType(EMoveType::NONE);
				}
			}
		}
	}
}

/*
void AGameField::SearchAttackPaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation, int32 Player)
{
	if (StepNumber < Troop.GetAttackRange())
	{
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (CurrentLocation.X - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)))
		{
			ATile* LeftTile = Field[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)];
			if (LeftTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != LeftTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SINISTRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(LeftTile), Troop.GetOwnerPlayer(), LeftTile->GetTroop()->GetOwnerPlayer());
				LeftTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y), Player);
			}
		}
		if (CurrentLocation.X + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)))
		{
			ATile* RightTile = Field[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)];
			if (RightTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != RightTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO DESTRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(RightTile), Troop.GetOwnerPlayer(), RightTile->GetTroop()->GetOwnerPlayer());
				RightTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y), Player);
			}
		}
		if (CurrentLocation.Y - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)))
		{
			ATile* UpperTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)];
			if (UpperTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != UpperTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SOPRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(UpperTile), Troop.GetOwnerPlayer(), UpperTile->GetTroop()->GetOwnerPlayer());
				UpperTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1), Player);
			}
		}
		if (CurrentLocation.Y + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)))
		{
			ATile* LowerTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)];
			if (LowerTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != LowerTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SOTTO - %s: Attaccante: %d, Difensore: %d"), *GetTileName(LowerTile), Troop.GetOwnerPlayer(), LowerTile->GetTroop()->GetOwnerPlayer());
				LowerTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1), Player);
			}
			else if (Troop.GetAttackType() == EAttackType::RANGED)
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1), Player);
			}
		}
	}
}*/

/*
void AGameField::SearchAttackPaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation, int32 Player)
{
	if (StepNumber < Troop.GetAttackRange())
	{
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (CurrentLocation.X - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)))
		{
			ATile* LeftTile = Field[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)];
			if (LeftTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != LeftTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SINISTRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(LeftTile), Troop.GetOwnerPlayer(), LeftTile->GetTroop()->GetOwnerPlayer());
				LeftTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y), Player);
			}
		}
		if (CurrentLocation.X + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)))
		{
			ATile* RightTile = Field[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)];
			if (RightTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != RightTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO DESTRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(RightTile), Troop.GetOwnerPlayer(), RightTile->GetTroop()->GetOwnerPlayer());
				RightTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y), Player);
			}
		}
		if (CurrentLocation.Y - 1 >= 0 && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)))
		{
			ATile* UpperTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)];
			if (UpperTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != UpperTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SOPRA - %s: Attaccante: %d, Difensore: %d"), *GetTileName(UpperTile), Troop.GetOwnerPlayer(), UpperTile->GetTroop()->GetOwnerPlayer());
				UpperTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1), Player);
			}
			else
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1), Player);
			}
		}
		if (CurrentLocation.Y + 1 < FieldSize && Field.Find(FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)))
		{
			ATile* LowerTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)];
			if (LowerTile->GetTileStatus() == ETileStatus::OCCUPIED && Troop.GetOwnerPlayer() != LowerTile->GetTroop()->GetOwnerPlayer())
			{
				UE_LOG(LogTemp, Log, TEXT("RILEVATO NEMICO SOTTO - %s: Attaccante: %d, Difensore: %d"), *GetTileName(LowerTile), Troop.GetOwnerPlayer(), LowerTile->GetTroop()->GetOwnerPlayer());
				LowerTile->SetMoveType(EMoveType::ATTACK);
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1), Player);
			}
			else if (Troop.GetAttackType() == EAttackType::RANGED)
			{
				SearchAttackPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1), Player);
			}
		}
	}
}
*/

void AGameField::ResetTilesMoveType()
{
	for (int32 I = 0; I < FieldSize; I++)
	{
		for (int32 J = 0; J < FieldSize; J++)
		{
			Field[FVector2D(I, J)]->SetMoveType(EMoveType::NONE); 
		}
	}
}

void AGameField::DrawPath(TArray<pair<int32, int32>>& Path)
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	for (int32 I = 0; I < Path.Num(); I++)
	{
		FVector2D TileLocation = FVector2D(Path[I].first, Path[I].second);
		Field[TileLocation]->SetPath(true);
		Field[TileLocation]->SetPathTurn(GamemodeBase->CurrentTurn);
	}
	SetTilesMaterial();
}

void AGameField::FilterAttackTiles()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	for (int32 I = 0; I < FieldSize; I++)
	{
		for (int32 J = 0; J < FieldSize; J++)
		{
			if (Field[FVector2D(I, J)]->GetMoveType() != EMoveType::ATTACK)
			{
				Field[FVector2D(I, J)]->SetMoveType(EMoveType::NONE);
			}
		}
	}
}

bool AGameField::IsValidPosition(int32 Col, int32 Row)
{
	return Row >= 0 && Row < FieldSize && Col >= 0 && Col < FieldSize;
}

bool AGameField::IsValidMovePosition(int32 Col, int32 Row)
{
	return IsValidPosition(Col, Row) && Field[FVector2D(Col, Row)]->GetMoveType() == EMoveType::MOVE;
}

bool AGameField::IsDestination(int32 Col, int32 Row, pair<int32, int32> Dest)
{
	return Col == Dest.first && Row == Dest.second;
}

void AGameField::InitialiseNodeMap(TMap<FVector2D, FNode*>& NodeMap, pair<int32, int32> Src)
{
	for (int32 Row = 0; Row < FieldSize; Row++)
	{
		for (int32 Col = 0; Col < FieldSize; Col++)
		{
			NodeMap.Add(FVector2D(Col, Row), new FNode());
			if (NodeMap[FVector2D(Col, Row)] == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Non ha creato il nodo"));
				return;
			}
		}
	}
	FVector2D SrcLocation = FVector2D(Src.first, Src.second);
	if (NodeMap.Contains(SrcLocation))
	{
		NodeMap[SrcLocation]->ParentCol = Src.first;
		NodeMap[SrcLocation]->ParentRow = Src.second;
		NodeMap[SrcLocation]->F = 0;
		NodeMap[SrcLocation]->G = 0;
		NodeMap[SrcLocation]->H = 0;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Non esiste il nodo Src"));
		return;
	}
}



void AGameField::InitialiseClosedSet(TMap<FVector2D, bool>& ClosedSet)
{
	for (int32 Row = 0; Row < FieldSize; Row++)
	{
		for (int32 Col = 0; Col < FieldSize; Col++)
		{
			ClosedSet.Add(FVector2D(Col, Row), false);
		}
	}
}

bool AGameField::IsBlockingEntity(int32 Col, int32 Row)
{
	return Field[FVector2D(Col, Row)]->GetTileStatus() != ETileStatus::FREE;
}

void AGameField::AStar(pair<int32, int32> Src, pair<int32, int32> Dest, TArray<pair<int32, int32>>& Path)
{
	if (!IsValidPosition(Src.first, Src.second))
	{
		UE_LOG(LogTemp, Error, TEXT("La sorgente è esterna al campo"));
		return;
	}

	if (!IsValidPosition(Dest.first, Dest.second))
	{
		UE_LOG(LogTemp, Error, TEXT("La destinazione è esterna al campo"));
		return;
	}

	TMap<FVector2D, FNode*> NodeMap;

	InitialiseNodeMap(NodeMap, Src);
	
	TMap<FVector2D, bool> ClosedSet;

	InitialiseClosedSet(ClosedSet);
	
	if (IsBlockingEntity(Dest.first, Dest.second))
	{
		UE_LOG(LogTemp, Error, TEXT("La destinazione non è una cella libera"));
		return;
	}

	TArray<pair<int32, pair<int32, int32>>> OpenSet;

	OpenSet.Add(make_pair(0.0, make_pair(Src.first, Src.second)));

	bool bFoundDestination = false;

	while (OpenSet.Num() > 0)
	{
		pair<double, pair<int32, int32>> HeadNode = OpenSet[0];
		OpenSet.RemoveAt(0);

		int32 CurrentCol = HeadNode.second.first;
		int32 CurrentRow = HeadNode.second.second;
		ClosedSet[FVector2D(CurrentCol, CurrentRow)] = true;

		double SuccessorG, SuccessorH, SuccessorF;

		//Primo successore (Sopra)
		if (IsValidMovePosition(CurrentCol - 1, CurrentRow))
		{
			if (IsDestination(CurrentCol - 1, CurrentRow, Dest))
			{
				NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->ParentRow = CurrentRow;
				NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->ParentCol = CurrentCol;
				GetShortestPath(NodeMap, Dest, Path);
				bFoundDestination = true;
				return;
			}
			else if (ClosedSet[FVector2D(CurrentCol - 1, CurrentRow)] == false && !IsBlockingEntity(CurrentCol - 1, CurrentRow))
			{
				SuccessorG = NodeMap[FVector2D(CurrentCol, CurrentRow)]->G + 1.f;
				SuccessorH = GetManhattanDistance(CurrentCol - 1, CurrentRow, Dest);
				SuccessorF = SuccessorG + SuccessorH;

				if (NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->F == FLT_MAX || NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->F > SuccessorF)
				{
					OpenSet.Add(make_pair(SuccessorF, make_pair(CurrentCol - 1, CurrentRow)));
					OpenSet.Sort([](const auto& A, const auto& B) { return A.first < B.first; });

					NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->H = SuccessorH;
					NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->G = SuccessorG;
					NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->F = SuccessorF;
					NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->ParentRow = CurrentRow;
					NodeMap[FVector2D(CurrentCol - 1, CurrentRow)]->ParentCol = CurrentCol;
				}
			}
		}

		//Secondo successore (Sotto)
		if (IsValidMovePosition(CurrentCol + 1, CurrentRow))
		{
			if (IsDestination(CurrentCol + 1, CurrentRow, Dest))
			{
				NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->ParentRow = CurrentRow;
				NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->ParentCol = CurrentCol;
				GetShortestPath(NodeMap, Dest, Path);
				bFoundDestination = true;
				return;
			}
			else if (ClosedSet[FVector2D(CurrentCol + 1, CurrentRow)] == false && !IsBlockingEntity(CurrentCol + 1, CurrentRow))
			{
				SuccessorG = NodeMap[FVector2D(CurrentCol, CurrentRow)]->G + 1.f;
				SuccessorH = GetManhattanDistance(CurrentCol + 1, CurrentRow, Dest);
				SuccessorF = SuccessorG + SuccessorH;

				if (NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->F == FLT_MAX || NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->F > SuccessorF)
				{
					OpenSet.Add(make_pair(SuccessorF, make_pair(CurrentCol + 1, CurrentRow)));
					OpenSet.Sort([](const auto& A, const auto& B) { return A.first < B.first; });

					NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->H = SuccessorH;
					NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->G = SuccessorG;
					NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->F = SuccessorF;
					NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->ParentRow = CurrentRow;
					NodeMap[FVector2D(CurrentCol + 1, CurrentRow)]->ParentCol = CurrentCol; 
				}
			}
		}

		//Terzo successore (Sinistra)
		if (IsValidMovePosition(CurrentCol, CurrentRow - 1))
		{
			if (IsDestination(CurrentCol, CurrentRow - 1, Dest))
			{
				NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->ParentRow = CurrentRow;
				NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->ParentCol = CurrentCol;
				GetShortestPath(NodeMap, Dest, Path);
				bFoundDestination = true;
				return;
			}
			else if (ClosedSet[FVector2D(CurrentCol, CurrentRow - 1)] == false && !IsBlockingEntity(CurrentCol, CurrentRow - 1))
			{
				SuccessorG = NodeMap[FVector2D(CurrentCol, CurrentRow)]->G + 1.f;
				SuccessorH = GetManhattanDistance(CurrentCol, CurrentRow - 1, Dest);
				SuccessorF = SuccessorG + SuccessorH;

				if (NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->F == FLT_MAX || NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->F > SuccessorF)
				{
					OpenSet.Add(make_pair(SuccessorF, make_pair(CurrentCol, CurrentRow - 1)));
					OpenSet.Sort([](const auto& A, const auto& B) { return A.first < B.first; });

					NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->H = SuccessorH;
					NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->G = SuccessorG;
					NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->F = SuccessorF;
					NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->ParentRow = CurrentRow;
					NodeMap[FVector2D(CurrentCol, CurrentRow - 1)]->ParentCol = CurrentCol;
				}
			}
		}

		//Quarto successore (Destra)
		if (IsValidMovePosition(CurrentCol, CurrentRow + 1))
		{
			if (IsDestination(CurrentCol, CurrentRow + 1, Dest))
			{
				NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->ParentRow = CurrentRow;
				NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->ParentCol = CurrentCol;
				GetShortestPath(NodeMap, Dest, Path);
				bFoundDestination = true;
				return;
			}
			else if (ClosedSet[FVector2D(CurrentCol, CurrentRow + 1)] == false && !IsBlockingEntity(CurrentCol, CurrentRow + 1))
			{
				SuccessorG = NodeMap[FVector2D(CurrentCol, CurrentRow)]->G + 1.f;
				SuccessorH = GetManhattanDistance(CurrentCol, CurrentRow + 1, Dest);
				SuccessorF = SuccessorG + SuccessorH;

				if (NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->F == FLT_MAX || NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->F > SuccessorF)
				{
					OpenSet.Add(make_pair(SuccessorF, make_pair(CurrentCol, CurrentRow + 1)));
					OpenSet.Sort([](const auto& A, const auto& B) { return A.first < B.first; }); 

					NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->H = SuccessorH;
					NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->G = SuccessorG;
					NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->F = SuccessorF;
					NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->ParentRow = CurrentRow;
					NodeMap[FVector2D(CurrentCol, CurrentRow + 1)]->ParentCol = CurrentCol; 
				}
			}
		}
	}
	if (!bFoundDestination)
	{
		UE_LOG(LogTemp, Log, TEXT("Destinazione non trovata"));
	}
}

void AGameField::GetShortestPath(TMap<FVector2D, FNode*>& NodeMap, pair<int32, int32> Dest, TArray<pair<int32, int32>>& Path)
{
	int32 Col = Dest.first;
	int32 Row = Dest.second;

	// Controlla che il nodo di destinazione esista nella mappa
	if (!NodeMap.Contains(FVector2D(Col, Row)))
	{
		return;
	}

	while (!(NodeMap[FVector2D(Col, Row)]->ParentRow == Row && NodeMap[FVector2D(Col, Row)]->ParentCol == Col))
	{
		Path.Add(make_pair(Col, Row));

		int32 PrevRow = Row;
		int32 PrevCol = Col;
		Row = NodeMap[FVector2D(PrevCol, PrevRow)]->ParentRow;
		Col = NodeMap[FVector2D(PrevCol, PrevRow)]->ParentCol;
	}

	// Aggiungi l'ultimo nodo di partenza
	Path.Add(make_pair(Col, Row));
}

int32 AGameField::GetTileSize()
{
	return TileSize;
}

ATile* AGameField::GetTile(int32 X, int32 Y)
{
	FVector2D FieldKey = FVector2D(X, Y);
	return Field[FieldKey];
}

ATile* AGameField::GetTileByRelativeLocation(FVector RelativeLocation)
{
	FVector Location = RelativeLocation / TileSize;
	return GetTile(Location.X, Location.Y);
}

FVector2D AGameField::GetGridLocationByRelativeLocation(FVector RelativeLocation)
{
	FVector Location = RelativeLocation / TileSize;
	int32 XPosition = FMath::RoundToInt(Location.X);
	int32 YPosition = FMath::RoundToInt(Location.Y);
	return FVector2D(XPosition, YPosition);
}

FVector AGameField::GetRelativeLocationByXY(int32 X, int32 Y)
{
	return TileSize * FVector(X, Y, 0);
}

FString AGameField::GetTileName(ATile* Tile)
{
	FVector2D TileCoordinates = Tile->GetGridLocation();
	int32 XPosition = FMath::RoundToInt(TileCoordinates.X);
	int32 YPosition = FMath::RoundToInt(TileCoordinates.Y + 1);

	FString Name = TEXT("");

	char Letter = static_cast<char>('A' + XPosition);
	Name += FString(1, &Letter);
	Name += FString::FromInt(YPosition);

	return Name;
}

int32 AGameField::GetFieldSize()
{
	return FieldSize;
}

double AGameField::GetManhattanDistance(int32 Row, int32 Col, pair<int32, int32> Dest)
{
	return FMath::Abs(Dest.first - Row) + FMath::Abs(Dest.second - Col);
}

void AGameField::ResetGameField()
{
	for (int32 I = 0; I < FieldSize; I++)
	{
		for (int32 J = 0; J < FieldSize; J++)
		{
			Field[FVector2D(I, J)]->ResetTile();
		}
	}
	GenerateObstacles();
	RefreshGameField();
}

void AGameField::BeginPlay()
{
	Super::BeginPlay();

	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	GamemodeBase->OnReset.AddDynamic(this, &AGameField::ResetGameField);

	GenerateField();
}

