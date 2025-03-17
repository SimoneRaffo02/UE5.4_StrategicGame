// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanPlayer.h"
#include "Tile.h"
#include "UEG_GamemodeBase.h"

// Sets default values
AHumanPlayer::AHumanPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	Camera->FieldOfView = 120;

	SetRootComponent(Camera);

	PlayerNumber = 0;
}

void AHumanPlayer::OnTurn()
{
	IsMyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnTurn!"));
}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHumanPlayer::OnClick()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnClick!"));
	FHitResult Hit = FHitResult(ForceInit);

	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);

	if (Hit.bBlockingHit && IsMyTurn)
	{
		AUEG_GamemodeBase* GamemodeBase = Cast<AUEG_GamemodeBase>(GetWorld()->GetAuthGameMode());
		if (GamemodeBase->bArcherPlacingTurn || GamemodeBase->bKnightPlacingTurn)
		{
			if (ATile* CurrentTile = Cast<ATile>(Hit.GetActor()))
			{
				if (CurrentTile->GetTileStatus() == ETileStatus::FREE)
				{
					IsMyTurn = false;
					CurrentTile->SetTileStatus(ETileStatus::OCCUPIED);
					FVector SpawnLocation = CurrentTile->GetActorLocation();
					GamemodeBase->PlaceTroop(PlayerNumber, SpawnLocation);
				}
			}
		}
		else if (GamemodeBase->IsGameStarted)
		{

		}
		else {

		}
	}
}

