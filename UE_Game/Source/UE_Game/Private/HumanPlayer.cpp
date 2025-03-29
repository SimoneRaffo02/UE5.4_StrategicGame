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
	Camera->SetRelativeRotation(FRotator(0, 0, 0));
	Camera->FieldOfView = 120;

	SetRootComponent(Camera);

	PlayerNumber = 0;

	SelectedTroop = nullptr;
}

void AHumanPlayer::OnTurn()
{
	IsMyTurn = true;
	Moved = false;
}

void AHumanPlayer::SetMoved(bool Value)
{
	Moved = Value;
}

bool AHumanPlayer::CanMove()
{
	return !Moved;
}

ATroop* AHumanPlayer::GetSelectedTroop()
{
	return SelectedTroop;
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
			//Selezione truppa
			if (ATroop* CurrentTroop = Cast<ATroop>(Hit.GetActor()))
			{
				if (Troops.Contains(CurrentTroop) && !Moved)
				{
					if (Troops.Contains(CurrentTroop))
					{
						if (CurrentTroop == SelectedTroop)
						{
							CurrentTroop->SetSelected(false);
							SelectedTroop = nullptr;
							GamemodeBase->GetGameField()->ResetTilesMoveType();
							GamemodeBase->GetGameField()->SetTilesMaterial();
							UE_LOG(LogTemp, Log, TEXT("Cliccato due volte"));
						}
						else
						{
							if (SelectedTroop != nullptr)
							{
								for (ATroop* Troop : Troops)
								{
									if (Troop->IsSelected())
									{
										Troop->SetSelected(false);
									}
								}
								GamemodeBase->GetGameField()->ResetTilesMoveType();
							}
							CurrentTroop->SetSelected(true);
							SelectedTroop = CurrentTroop;
							GamemodeBase->GetGameField()->EvaluatePossibleMoves(CurrentTroop);
							GamemodeBase->GetGameField()->SetTilesMaterial();
						}
					}
				}
			}
			else if (ATile* CurrentTile = Cast<ATile>(Hit.GetActor()))
			{
				if (SelectedTroop != nullptr)
				{
					if (CurrentTile->GetMoveType() == EMoveType::MOVE && !Moved)
					{
						Moved = true; 
						TArray<pair<int32, int32>> Path;
						pair<int32, int32> Src = make_pair(GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).X, GamemodeBase->GetGameField()->GetGridLocationByRelativeLocation(SelectedTroop->GetActorLocation()).Y);
						pair<int32, int32> Dest = make_pair(CurrentTile->GetGridLocation().X, CurrentTile->GetGridLocation().Y);
						GamemodeBase->GetGameField()->AStar(Src, Dest, Path);
						GamemodeBase->GetGameField()->DrawPath(Path);
						SelectedTroop->SetCurrentPath(Path);
						GamemodeBase->GetGameField()->ResetTilesMoveType();
						GamemodeBase->GetGameField()->SetTilesMaterial();
					}
					if (CurrentTile->GetMoveType() == EMoveType::ATTACK)
					{

					}
				}
			}
		}
		else {
			
		}
	}
}

