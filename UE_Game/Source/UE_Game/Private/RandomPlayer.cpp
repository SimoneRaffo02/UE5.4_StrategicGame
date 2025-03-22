// Fill out your copyright notice in the Description page of Project Settings.

#include "RandomPlayer.h"
#include "UEG_GamemodeBase.h"
#include "GameField.h"

// Sets default values
ARandomPlayer::ARandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARandomPlayer::OnTurn()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]() 
	{
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GamemodeBase->bArcherPlacingTurn || GamemodeBase->bKnightPlacingTurn)
		{
			int32 XPosition;
			int32 YPosition;
			bool bFreeTile = false;
			do {
				XPosition = FMath::RandRange(0, GamemodeBase->GetFieldSize() - 1);
				YPosition = FMath::RandRange(0, GamemodeBase->GetFieldSize() - 1);
				if (GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->GetTileStatus() == ETileStatus::FREE)
				{
					bFreeTile = true;
				}
			} while (!bFreeTile);
			FVector SpawnLocation = GamemodeBase->GetGameField()->GetRelativeLocationByXY(XPosition, YPosition);
			GamemodeBase->PlaceTroop(PlayerNumber, SpawnLocation);
			GamemodeBase->GetGameField()->GetTile(XPosition, YPosition)->SetTileStatus(ETileStatus::OCCUPIED);
		}
		else if (GamemodeBase->IsGameStarted)
		{
			
		}
		else {

		}
	}, 1, false);
}

// Called when the game starts or when spawned
void ARandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

