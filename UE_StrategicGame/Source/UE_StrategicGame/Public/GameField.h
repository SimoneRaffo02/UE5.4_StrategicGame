#pragma once

#include "Tile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameField.generated.h"

UCLASS()
class UE_STRATEGICGAME_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	AGameField();

	void GenerateField();

	void SetFieldSize(int32 NewFieldSize);

	int32 GetTileSize();

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
};
