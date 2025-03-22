#include "Tile.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ImageStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImageStaticMesh"));

	SetRootComponent(Scene);
	StaticMesh->SetupAttachment(Scene);
	ImageStaticMesh->SetupAttachment(Scene);

	FloorMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	MountainMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	MoveMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	AttackMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material")); 
	CurrentPositionMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));

	Status = ETileStatus::FREE;
	GridLocation = FVector2D(0, 0);
	Troop = nullptr;
	MoveType = "None";
}

void ATile::SetTileStatus(ETileStatus NewStatus)
{
	Status = NewStatus;
}

void ATile::SetGridLocation(int32 X, int32 Y)
{
	GridLocation = FVector2D(X, Y);
}

void ATile::SetTroop(ATroop& NewTroop)
{
	Troop = &NewTroop;
}

void ATile::SetMoveType(FString NewMoveType)
{
	if (NewMoveType == "None" || NewMoveType == "Move" || NewMoveType == "Attack" || NewMoveType == "CurrentPosition")
	{
		MoveType = NewMoveType;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Move Type"));
	}
}

ETileStatus ATile::GetTileStatus()
{
	return Status;
}

FVector2D ATile::GetGridLocation()
{
	return GridLocation;
}

ATroop* ATile::GetTroop()
{
	return Troop;
}

FString ATile::GetMoveType()
{
	return MoveType;
}

void ATile::SetTilesMaterial()
{
	if (MoveType == "None" && Status != ETileStatus::OBSTACLE)
	{
		ImageStaticMesh->SetMaterial(0, FloorMaterial);
	}
	else if (MoveType == "None" && Status == ETileStatus::OBSTACLE)
	{
		ImageStaticMesh->SetMaterial(0, MountainMaterial);
	}
	else if (MoveType == "Move")
	{
		ImageStaticMesh->SetMaterial(0, MoveMaterial);
	}
	else if (MoveType == "Attack")
	{
		ImageStaticMesh->SetMaterial(0, AttackMaterial);
	}
	else if (MoveType == "CurrentPosition")
	{
		ImageStaticMesh->SetMaterial(0, CurrentPositionMaterial);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}



