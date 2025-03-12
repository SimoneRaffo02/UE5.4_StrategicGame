#include "Tile.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SetRootComponent(Scene);
	StaticMesh->SetupAttachment(Scene);

	Status = ETileStatus::FREE;
	GridLocation = FVector2D(0, 0);
	Troop = nullptr;
}

void ATile::SetTileStatus(ETileStatus NewStatus)
{
	Status = NewStatus;
}

void ATile::SetGridLocation(int32 X, int32 Y)
{
	GridLocation = FVector2D(X, Y);
}

void ATile::SetTroop(TSubclassOf<ATroop> NewTroop)
{
	Troop = NewTroop;
}

ETileStatus ATile::GetTileStatus()
{
	return Status;
}

FVector2D ATile::GetGridLocation()
{
	return GridLocation;
}

TSubclassOf<ATroop> ATile::GetTroop()
{
	return Troop;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

}



