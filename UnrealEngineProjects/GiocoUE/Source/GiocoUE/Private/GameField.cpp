#include "GameField.h"
#include <iostream>

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

void AGameField::SetFieldSize(int32 NewFieldSize)
{
	FieldSize = NewFieldSize;
}

int32 AGameField::GetTileSize()
{
	return TileSize;
}

FVector AGameField::GetRelativeLocationByXY(int32 X, int32 Y)
{
	return 100 * FVector(X, Y, 0);
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

