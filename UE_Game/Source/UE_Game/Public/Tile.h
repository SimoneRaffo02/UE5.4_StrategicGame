#pragma once

#include "Troop.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class ETileStatus : uint8
{
	FREE		UMETA(DisplayName = "FREE"),
	OCCUPIED	UMETA(DisplayName = "OCCUPIED"),
	OBSTACLE    UMETA(DisplayName = "OBSTACLE")
};

UCLASS()
class UE_GAME_API ATile : public AActor
{
	GENERATED_BODY()

public:
	ATile();

	void SetTileStatus(ETileStatus NewStatus);

	void SetGridLocation(int32 X, int32 Y);

	void SetTroop(ATroop& Troop);

	void SetMoveType(FString NewMoveType);

	ETileStatus GetTileStatus();

	FVector2D GetGridLocation();

	ATroop* GetTroop();

	FString GetMoveType();

	void SetTilesMaterial();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ImageStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* FloorMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* MountainMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* MoveMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* AttackMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* CurrentPositionMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FVector2D GridLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ATroop* Troop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FString MoveType;
};
