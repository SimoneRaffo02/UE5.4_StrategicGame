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

UENUM()
enum class EMoveType : uint8
{
	NONE		UMETA(DisplayName = "NONE"),
	MOVE		UMETA(DisplayName = "MOVE"),
	ATTACK		UMETA(DisplayName = "ATTACK")
};

UCLASS()
class UE_GAME_API ATile : public AActor
{
	GENERATED_BODY()

public:
	ATile();

	void SetTileStatus(ETileStatus NewStatus);

	void SetGridLocation(int32 X, int32 Y);

	void SetTroop(ATroop* Troop);

	void SetMoveType(EMoveType NewMoveType);

	void SetPath(bool Value);

	void SetPathTurn(int32 NewPathTurn);

	ETileStatus GetTileStatus();

	FVector2D GetGridLocation();

	ATroop* GetTroop();

	EMoveType GetMoveType();

	bool IsPath();

	int32 GetPathTurn();

	void SetTilesMaterial();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ImageStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PathStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* FloorMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* ObstacleMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* MoveMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* AttackMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* PathMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FVector2D GridLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ATroop* Troop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	EMoveType MoveType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	bool bPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	int32 PathTurn;
};
