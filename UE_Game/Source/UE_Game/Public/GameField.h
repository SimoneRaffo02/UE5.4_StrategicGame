#pragma once

#include <utility>
#include "Tile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameField.generated.h"

using namespace std;

USTRUCT(BlueprintType)
struct FNode
{
	GENERATED_BODY()

	UPROPERTY()
	int32 ParentRow;

	UPROPERTY()
	int32 ParentCol;

	UPROPERTY()
	float F;

	UPROPERTY()
	float G;

	UPROPERTY()
	float H;

	// Costruttore
	FNode()
		: ParentRow(-1), ParentCol(-1), F(FLT_MAX), G(FLT_MAX), H(FLT_MAX) {
	}
};

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

	void SearchMovePaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation);

	void SearchAttackPaths(int32 StepNumber, ATroop& Troop, FVector2D CurrentLocation);

	void ResetTilesMoveType();

	void DrawPath(TArray<pair<int32, int32>>& Path);

	void FilterAttackTiles();

	bool IsValidPosition(int32 Col, int32 Row);

	bool IsValidMovePosition(int32 Col, int32 Row);

	bool IsDestination(int32 Col, int32 Row, pair<int32, int32> Dest);

	void InitialiseNodeMap(TMap<FVector2D, FNode*>& NodeMap, pair<int32, int32> Src);

	void InitialiseClosedSet(TMap<FVector2D, bool>& ClosedSet);

	bool IsBlockingEntity(int32 Col, int32 Row);

	void AStar(pair<int32, int32> Src, pair<int32, int32> Dest, TArray<pair<int32, int32>>& Path);

	void GetShortestPath(TMap<FVector2D, FNode*>& NodeMap, pair<int32, int32> Dest, TArray<pair<int32, int32>>& Path);

	int32 GetTileSize();

	ATile* GetTile(int32 X, int32 Y);

	ATile* GetTileByRelativeLocation(FVector RelativeLocation);

	FVector2D GetGridLocationByRelativeLocation(FVector RelativeLocation);

	FVector GetRelativeLocationByXY(int32 X, int32 Y);

	FVector2D GetPosition(const FHitResult& Hit);

	double GetManhattanDistance(int32 Row, int32 Col, pair<int32, int32> Dest);

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
