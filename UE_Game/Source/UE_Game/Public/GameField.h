#pragma once

#include "Tile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameField.generated.h"

UCLASS()
class UE_GAME_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	AGameField();

	void GenerateField();

	void GenerateObstacles();

	void SetFieldSize(int32 NewFieldSize);

	void SetObstaclesPercentage(float NewObstaclesPercentage);

	void SetTilesMaterial();

	bool CheckUnreachableTiles(int32 XPosition, int32 YPosition);

	void UpdateAnalysisField(TMap<FVector2D, TArray<bool>>& AnalysisField, FVector2D CurrentLocation);

	void EvaluatePossibleMoves(ATroop* Troop);

	void SearchPaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation);

	void ResetTilesMoveType();

	int32 GetTileSize();

	ATile* GetTile(int32 X, int32 Y);

	ATile* GetTileByRelativeLocation(FVector RelativeLocation);

	FVector2D GetGridLocationByRelativeLocation(FVector RelativeLocation);

	FVector GetRelativeLocationByXY(int32 X, int32 Y);

	FVector2D GetPosition(const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> Field;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 TileSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ObstaclesPercentage;
};
