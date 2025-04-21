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
class GIOCOUE_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATile();

	void SetTileStatus(ETileStatus NewStatus);

	void SetGridLocation(int32 X, int32 Y);

	void SetTroop(TSubclassOf<ATroop>);

	ETileStatus GetTileStatus();

	FVector2D GetGridLocation();

	TSubclassOf<ATroop> GetTroop();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	FVector2D GridLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	TSubclassOf<ATroop> Troop;

};
