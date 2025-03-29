#include "Tile.h"
#include "UEG_GamemodeBase.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ImageStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImageStaticMesh")); 
	PathStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PathStaticMesh"));

	SetRootComponent(Scene);
	StaticMesh->SetupAttachment(Scene);
	ImageStaticMesh->SetupAttachment(Scene);
	PathStaticMesh->SetupAttachment(Scene);

	FloorMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	ObstacleMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	MoveMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
	AttackMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material")); 
	PathMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));

	Status = ETileStatus::FREE;
	GridLocation = FVector2D(0, 0);
	Troop = nullptr;
	MoveType = EMoveType::NONE; 
	bPath = false;
	PathTurn = -1;
}

void ATile::SetTileStatus(ETileStatus NewStatus)
{
	Status = NewStatus;
}

void ATile::SetGridLocation(int32 X, int32 Y)
{
	GridLocation = FVector2D(X, Y);
}

void ATile::SetTroop(ATroop* NewTroop)
{
	Troop = NewTroop;
}

void ATile::SetMoveType(EMoveType NewMoveType)
{
	MoveType = NewMoveType;
}

void ATile::SetPath(bool Value)
{
	bPath = Value;
}

void ATile::SetPathTurn(int32 NewPathTurn)
{
	PathTurn = NewPathTurn;
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

EMoveType ATile::GetMoveType()
{
	return MoveType;
}

bool ATile::IsPath()
{
	return bPath;
}

int32 ATile::GetPathTurn()
{
	return PathTurn;
}

void ATile::SetTilesMaterial()
{
	AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
	if (MoveType == EMoveType::NONE && Status != ETileStatus::OBSTACLE)
	{
		ImageStaticMesh->SetMaterial(0, FloorMaterial);
	}
	else if (MoveType == EMoveType::NONE && Status == ETileStatus::OBSTACLE)
	{
		ImageStaticMesh->SetMaterial(0, ObstacleMaterial);
	}
	else if (MoveType == EMoveType::MOVE)
	{
		ImageStaticMesh->SetMaterial(0, MoveMaterial);
	}
	else if (MoveType == EMoveType::ATTACK)
	{
		ImageStaticMesh->SetMaterial(0, AttackMaterial);
	}
	if (bPath && PathTurn < GamemodeBase->CurrentTurn + 1)
	{
		PathStaticMesh->SetVisibility(true);
	}
	else
	{
		PathStaticMesh->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	PathStaticMesh->SetMaterial(0, PathMaterial);
	PathStaticMesh->SetVisibility(false);
}



