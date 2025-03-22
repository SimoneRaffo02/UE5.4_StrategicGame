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
	SearchPaths(0, *Troop, TroopLocation);
	SetTilesMaterial();
}

void AGameField::SearchPaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation)
{
	if (StepNumber < Troop.GetMovement())
	{

		if (Field[CurrentLocation]->GetTileStatus() == ETileStatus::OCCUPIED && Field[CurrentLocation]->GetTroop() != &Troop)
		{
			return;
		}
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (CurrentLocation.X - 1 >= 0)
		{
			ATile* LeftTile = Field[FVector2D(CurrentLocation.X - 1, CurrentLocation.Y)];
			if (LeftTile->GetTileStatus() == ETileStatus::FREE)
			{
				LeftTile->SetMoveType("Move");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y));
			}
			else if (LeftTile->GetTileStatus() == ETileStatus::OCCUPIED && GamemodeBase->Players[GamemodeBase->GetNextPlayer(GamemodeBase->CurrentPlayer)]->GetTroops().Contains(LeftTile->GetTroop()))
			{
				LeftTile->SetMoveType("Attack");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X - 1, CurrentLocation.Y));
			}
		}
		if (CurrentLocation.X + 1 < FieldSize)
		{
			ATile* RightTile = Field[FVector2D(CurrentLocation.X + 1, CurrentLocation.Y)];
			if (RightTile->GetTileStatus() == ETileStatus::FREE)
			{
				RightTile->SetMoveType("Move");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y));
			}
			else if (RightTile->GetTileStatus() == ETileStatus::OCCUPIED && GamemodeBase->Players[GamemodeBase->GetNextPlayer(GamemodeBase->CurrentPlayer)]->GetTroops().Contains(RightTile->GetTroop()))
			{
				RightTile->SetMoveType("Attack");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X + 1, CurrentLocation.Y));
			}
		}
		if (CurrentLocation.Y - 1 >= 0)
		{
			ATile* UpperTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y - 1)];
			if (UpperTile->GetTileStatus() == ETileStatus::FREE)
			{
				UpperTile->SetMoveType("Move");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1));
			}
			else if (UpperTile->GetTileStatus() == ETileStatus::OCCUPIED && GamemodeBase->Players[GamemodeBase->GetNextPlayer(GamemodeBase->CurrentPlayer)]->GetTroops().Contains(UpperTile->GetTroop()))
			{
				UpperTile->SetMoveType("Attack");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y - 1));
			}
		}
		if (CurrentLocation.Y + 1 < FieldSize)
		{
			ATile* LowerTile = Field[FVector2D(CurrentLocation.X, CurrentLocation.Y + 1)];
			if (LowerTile->GetTileStatus() == ETileStatus::FREE)
			{
				LowerTile->SetMoveType("Move");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1));
			}
			else if (LowerTile->GetTileStatus() == ETileStatus::OCCUPIED && GamemodeBase->Players[GamemodeBase->GetNextPlayer(GamemodeBase->CurrentPlayer)]->GetTroops().Contains(LowerTile->GetTroop()))
			{
				LowerTile->SetMoveType("Attack");
				SearchPaths(StepNumber + 1, Troop, FVector2D(CurrentLocation.X, CurrentLocation.Y + 1));
			}
		}
	}
	if (Field[CurrentLocation]->GetTroop() == &Troop)
	{
		Field[CurrentLocation]->SetMoveType("CurrentPosition");
		UE_LOG(LogTemp, Log, TEXT("Blue"));
	}
}

void AGameField::ResetTilesMoveType()
{
	for (int32 I = 0; I < FieldSize; I++)
	{
		for (int32 J = 0; J < FieldSize; J++)
		{
			Field[FVector2D(I, J)]->SetMoveType("None");
		}
	}
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
	return FVector2D(Location.X, Location.Y);
}

FVector AGameField::GetRelativeLocationByXY(int32 X, int32 Y)
{
	return TileSize * FVector(X, Y, 0);
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridLocation();
}

void AGameField::BeginPlay()
{
	Super::BeginPlay();
	AGameField::GenerateField();
}

